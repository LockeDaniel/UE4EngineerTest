// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotInstruction.h"
#include "Robot.h"
#include "RobotSphere.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "DrawDebugHelpers.h"
#include "Misc/FileHelper.h"

// -------------------------------------
// Robot instruction base
// -------------------------------------
URobotInstruction::URobotInstruction()
{
	bIsInitialized = false;
}

void URobotInstruction::Initialize(ARobot* ParentRobot)
{
	Robot = ParentRobot;
	bIsInitialized = true;
}

bool URobotInstruction::ExecuteInstruction(float DeltaTime)
{
	return true;
}
// -------------------------------------

// -------------------------------------
// Move forward instruction
// -------------------------------------
URInstructionMoveForward::URInstructionMoveForward()
	: URobotInstruction()
{
}

void URInstructionMoveForward::Initialize(ARobot* ParentRobot)
{
	Super::Initialize(ParentRobot);

	StartLocation = ParentRobot->GetActorLocation();
}

bool URInstructionMoveForward::ExecuteInstruction(float DeltaTime)
{
	if (Robot)
	{
		FVector Forward = Robot->GetActorForwardVector();
		FVector Location = Robot->GetActorLocation();

		// Check for collision
		if (auto World = Robot->GetWorld())
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Robot);
			TArray<FHitResult> Hits;
			float HalfHeight = Robot->WallCheckDistance / 2.0f;
			float Radius = Robot->WallCheckWidth / 2.0f;
			FVector CapsuleLocation = Location + Forward * HalfHeight;
			FRotator CapsuleRotation = FRotator(90.0f, 0.0f, 0.0f) + Robot->GetActorRotation();

			bool bGotHit = World->SweepMultiByChannel(
				Hits,
				CapsuleLocation,
				CapsuleLocation + Forward,
				FQuat(CapsuleRotation),
				ECC_Visibility,
				FCollisionShape::MakeCapsule(Radius, HalfHeight),
				QueryParams
			);

			if (Robot->bWallCheckDebug)
			{
				UKismetSystemLibrary::DrawDebugCapsule(
					World, 
					CapsuleLocation,
					HalfHeight,
					Radius,
					CapsuleRotation,
					FLinearColor::White,
					0.0f,
					5.0f
				);
			}

			if (bGotHit)
			{
				for (auto Hit : Hits)
				{
					// Avoid obstacle
					if (Hit.Actor != Robot && Cast<ARobotSphere>(Hit.Actor) == nullptr)
					{
						FVector HitNormal = FVector::VectorPlaneProject(Hit.Normal, FVector::UpVector);
						HitNormal.Normalize();

						if (HitNormal.ContainsNaN())
							HitNormal = -Forward;

						// Get the amount we need to rotate in order to avoid the obstacle
						float RotateAmount = FMath::FindDeltaAngleDegrees(
							Forward.Rotation().Yaw,
							Forward.MirrorByVector(HitNormal).Rotation().Yaw
						);

						// Clamp to remove oscillations
						RotateAmount = FMath::ClampAngle(RotateAmount, -120.0f, 120.0f);
						if (RotateAmount < 1.0f && RotateAmount > -1.0f)
							RotateAmount = 1.0f;

						Robot->ClearInstructions();
						Robot->bLastWallBounceClockWise = RotateAmount >= 0.0f;
						Robot->AddInstruction(URInstructionRotate::CreateInstructionRotate(RotateAmount));
						Robot->AddInstruction(URInstructionMoveForward::CreateInstructionMoveForward(200.0f));

						return true;
					}
				}
			}
		}
		else
			return true;

		// Keep going
		float DistanceTraveled = GetDistanceTraveled();
		float DistanceToGo = Distance - DistanceTraveled;
		float Speed = FMath::Min(Robot->MoveSpeed * DeltaTime, Robot->WallCheckDistance / 2.0f);

		if (DistanceToGo > Speed)
		{
			Robot->SetActorLocation(Location + Forward * Speed);
			//Robot->RobotMesh->SetPhysicsLinearVelocity(Forward * (Robot->Speed * DeltaTime), true);

			return false;
		}
		else
		{
			Robot->SetActorLocation(Location + Forward * DistanceToGo);
		}
	}

	return true;
}

float URInstructionMoveForward::GetDistanceTraveled()
{
	return (Robot->GetActorLocation() - StartLocation).Size();
}
// -------------------------------------

// -------------------------------------
// Rotate instruction
// -------------------------------------
URInstructionRotate::URInstructionRotate()
	: URobotInstruction()
{
	TargetYaw = 0.0f;
	RotateDirection = 1.0f;
}

void URInstructionRotate::Initialize(ARobot* ParentRobot)
{
	Super::Initialize(ParentRobot);

	TargetYaw = ParentRobot->GetActorRotation().Yaw + RotateDegrees;
	RotateDirection = RotateDegrees < 0 ? -1.0f : 1.0f;
}

bool URInstructionRotate::ExecuteInstruction(float DeltaTime)
{
	if (Robot)
	{
		FRotator Rotation = Robot->GetActorRotation();
		float Yaw = Robot->GetActorRotation().Yaw;
		float YawDelta = FMath::FindDeltaAngleDegrees(Yaw, TargetYaw);
		float Speed = Robot->RotateSpeed * DeltaTime;

		if (FMath::Abs(YawDelta) > Speed)
		{
			Rotation.Yaw = Yaw + (Speed * RotateDirection);
			Robot->SetActorRotation(Rotation);

			return false;
		}
		else
		{
			Rotation.Yaw = Yaw + YawDelta;
			Robot->SetActorRotation(Rotation);
		}
	}

	return true;
}
// -------------------------------------

// -------------------------------------
// Wait instruction
// -------------------------------------
URInstructionWait::URInstructionWait()
	: URobotInstruction()
{
	Duration = 0.0f;
	ElapsedTime = 0.0f;
}

void URInstructionWait::Initialize(ARobot* ParentRobot)
{
	Super::Initialize(ParentRobot);
}

bool URInstructionWait::ExecuteInstruction(float DeltaTime)
{
	if (Robot)
	{
		ElapsedTime += DeltaTime;
		if (ElapsedTime < Duration)
		{
			return false;
		}
	}

	return true;
}
// -------------------------------------

// -------------------------------------
// Check for spheres instruction
// -------------------------------------
URInstructionCheckForSpheres::URInstructionCheckForSpheres()
	: URobotInstruction()
{
}

void URInstructionCheckForSpheres::Initialize(ARobot* ParentRobot)
{
	Super::Initialize(ParentRobot);
}

bool URInstructionCheckForSpheres::ExecuteInstruction(float DeltaTime)
{
	if (Robot)
	{
		if (auto World = Robot->GetWorld())
		{
			FVector RobotLocation = Robot->GetActorLocation();
			FVector Forward = Robot->GetActorForwardVector();

			ARobotSphere* ClosestSphere = nullptr;
			float ClosestSphereDistance = 0.0f;
			float ClosestSphereLookAtYaw = 0.0f;
			FVector ClosestSphereHitLocation = FVector::ZeroVector;

			//World->GetFirstPlayerController()->ProjectWorldLocationToScreen()
			for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
			{
				auto SphereActor = Cast<ARobotSphere>(*ActorItr);
				if (!SphereActor)
					continue;

				FVector SphereLocation = SphereActor->GetActorLocation();

				FVector LocationDif = SphereLocation - RobotLocation;
				FVector SphereDirection = LocationDif;
				SphereDirection.Normalize();
				//float ActorAngle = (1.0f - FVector::DotProduct(Forward, SphereDirection)) * 180.0f;
				float SphereLookAtYaw = SphereDirection.Rotation().Yaw - Robot->GetActorRotation().Yaw;

				if (FMath::Abs(SphereLookAtYaw) <= Robot->Camera->FieldOfView / 2.0f)
				{
					// Make sure the sphere isn't occluded.
					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(Robot);
					FHitResult Hit;
					bool bGotHit = World->LineTraceSingleByChannel(
						Hit,
						RobotLocation,
						SphereLocation,
						ECC_Visibility,
						QueryParams
					);

					if (bGotHit && Hit.Actor.IsValid() && Hit.Actor == SphereActor)
					{
						float Distance = (Hit.Location - RobotLocation).Size();
						if (!ClosestSphere || Distance < ClosestSphereDistance)
						{
							ClosestSphere = SphereActor;
							ClosestSphereDistance = Distance;
							ClosestSphereLookAtYaw = SphereLookAtYaw;
							ClosestSphereHitLocation = Hit.Location;
						}
					}
				}
			}

			if (ClosestSphere)
			{
				Robot->ClearInstructions();

				// Rotate to face the sphere
				Robot->AddInstruction(URInstructionRotate::CreateInstructionRotate(ClosestSphereLookAtYaw));

				// Go to the sphere
				float Distance = (ClosestSphereHitLocation - RobotLocation).Size() - Robot->RobotMesh->Bounds.SphereRadius;
				Robot->AddInstruction(URInstructionMoveForward::CreateInstructionMoveForward(Distance - Robot->SphereDocumentDistance));

				// Document the sphere
				Robot->AddInstruction(URInstructionDocumentSphere::CreateInstructionDocumentSphere(ClosestSphere));
			}
		}
	}

	return true;
}
// -------------------------------------

// -------------------------------------
// Document sphere instruction
// -------------------------------------
URInstructionDocumentSphere::URInstructionDocumentSphere()
	: URobotInstruction()
{
	Sphere = nullptr;
}

void URInstructionDocumentSphere::Initialize(ARobot* ParentRobot)
{
	Super::Initialize(ParentRobot);
}

bool URInstructionDocumentSphere::ExecuteInstruction(float DeltaTime)
{
	if (Robot && Sphere)
	{
		if (auto World = Robot->GetWorld())
		{
			FVector RobotLocation = Robot->GetActorLocation();
			FVector SphereLocation = Sphere->GetActorLocation();
			FVector Forward = Robot->GetActorForwardVector();

			// Make sure the sphere isn't occluded.
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Robot);
			FHitResult Hit;
			bool bGotHit = World->LineTraceSingleByChannel(
				Hit,
				RobotLocation,
				SphereLocation,
				ECC_Visibility,
				QueryParams
			);

			if (bGotHit && Hit.Actor.IsValid() && Hit.Actor == Sphere)
			{
				float SphereDistance = (Hit.Location - RobotLocation).Size() - Robot->RobotMesh->Bounds.SphereRadius;
				if (SphereDistance < Robot->SphereDocumentDistanceMax)
				{
					auto OutputLocation = FPaths::GetPath(FPaths::GetProjectFilePath()) + FString("/RobotData/");

					// Take a screenshot
					Robot->SceneCapture->CaptureScene();
					UKismetRenderingLibrary::ExportRenderTarget(
						World,
						Robot->RenderTarget,
						OutputLocation,
						FString("image_") + FString::FromInt(Robot->ScreenshotCounter) + FString(".png")
					);

					// Output actor log
					FString ActorLog = FString();
					for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
					{
						auto Actor = *ActorItr;

						FVector ActorLocation = Actor->GetActorLocation();
						FVector LocationDif = ActorLocation - RobotLocation;
						FVector ActorDirection = LocationDif;
						ActorDirection.Normalize();
						float ActorAngle = (1.0f - FVector::DotProduct(Forward, ActorDirection)) * 180.0f;

						if (FMath::Abs(ActorAngle) <= Robot->SceneCapture->FOVAngle / 2.0f)
						{
							bGotHit = World->LineTraceSingleByChannel(
								Hit,
								RobotLocation,
								ActorLocation,
								ECC_Visibility,
								QueryParams
							);

							if (bGotHit && Hit.Actor.IsValid() && Hit.Actor == Actor)
							{
								ActorLog += Actor->GetName() + "\n";
							}
						}
					}
					FString ActorLogFile = OutputLocation + FString("image_") + FString::FromInt(Robot->ScreenshotCounter) + FString(".txt");
					FFileHelper::SaveStringToFile(
						ActorLog,
						*ActorLogFile
					);

					Sphere->Destroy();

					Robot->ScreenshotCounter++;
				}
			}
		}
	}

	return true;
}
// -------------------------------------

// -------------------------------------
// Drop sphere instruction
// -------------------------------------
URInstructionDropSphere::URInstructionDropSphere()
	: URobotInstruction()
{
}

void URInstructionDropSphere::Initialize(ARobot* ParentRobot)
{
	Super::Initialize(ParentRobot);
}

bool URInstructionDropSphere::ExecuteInstruction(float DeltaTime)
{
	if (Robot)
	{
		if (auto World = Robot->GetWorld())
		{
			FVector RobotLocation = Robot->GetActorLocation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			auto NewSphere = World->SpawnActor<ARobotSphere>(
				Robot->SphereType,
				RobotLocation - Robot->GetActorForwardVector() * Robot->SphereDocumentDistance,
				FRotator::ZeroRotator,
				SpawnParams
			);
		}
	}

	return true;
}
// -------------------------------------