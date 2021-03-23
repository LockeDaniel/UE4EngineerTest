// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RobotInstruction.generated.h"

class ARobot;

/**
 * Base instruction class.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URobotInstruction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="RobotInstruction")
		ARobot* Robot;

protected:
	bool bIsInitialized;

public:
	URobotInstruction();	

	virtual void Initialize(ARobot* ParentRobot);

	UFUNCTION(BlueprintCallable)
		virtual bool ExecuteInstruction(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		bool IsInitialized()
		{
			return bIsInitialized;
		}
};

/**
 * Instruction to move the robot forward.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URInstructionMoveForward : public URobotInstruction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="RobotInstruction")
		float Distance;

protected:
	FVector StartLocation;

public:
	URInstructionMoveForward();

	UFUNCTION(BlueprintCallable)
		static URInstructionMoveForward* CreateInstructionMoveForward(float DistanceToTravel)
		{
			auto Instruction = NewObject<URInstructionMoveForward>();
			Instruction->Distance = DistanceToTravel;

			return Instruction;
		}

	void Initialize(ARobot* ParentRobot) override;
	bool ExecuteInstruction(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "RobotInstruction")
		float GetDistanceTraveled();
};

/**
 * Instruction to rotate the robot.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URInstructionRotate : public URobotInstruction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "RobotInstruction")
		float RotateDegrees;

protected:
	float TargetYaw;
	float RotateDirection;

public:
	URInstructionRotate();

	UFUNCTION(BlueprintCallable)
		static URInstructionRotate* CreateInstructionRotate(float RotateAmountDegrees)
		{
			auto Instruction = NewObject<URInstructionRotate>();
			Instruction->RotateDegrees = RotateAmountDegrees;

			return Instruction;
		}

	void Initialize(ARobot* ParentRobot) override;
	bool ExecuteInstruction(float DeltaTime) override;
};

/**
 * Instruction for the robot to wait.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URInstructionWait : public URobotInstruction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "RobotInstruction")
		float Duration;

	UPROPERTY(BlueprintReadOnly, Category = "RobotInstruction")
		float ElapsedTime;

public:
	URInstructionWait();

	UFUNCTION(BlueprintCallable)
		static URInstructionWait* CreateInstructionWait(float WaitDuration)
		{
			auto Instruction = NewObject<URInstructionWait>();
			Instruction->Duration = WaitDuration;

			return Instruction;
		}

	void Initialize(ARobot* ParentRobot) override;
	bool ExecuteInstruction(float DeltaTime) override;
};

/**
 * Instruction to check for spheres.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URInstructionCheckForSpheres : public URobotInstruction
{
	GENERATED_BODY()

public:

public:
	URInstructionCheckForSpheres();

	UFUNCTION(BlueprintCallable)
		static URInstructionCheckForSpheres* CreateInstructionCheckForSpheres()
		{
			auto Instruction = NewObject<URInstructionCheckForSpheres>();

			return Instruction;
		}

	void Initialize(ARobot* ParentRobot) override;
	bool ExecuteInstruction(float DeltaTime) override;
};

/**
 * Instruction to document a sphere.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URInstructionDocumentSphere : public URobotInstruction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "RobotInstruction")
		ARobotSphere* Sphere;

public:
	URInstructionDocumentSphere();

	UFUNCTION(BlueprintCallable)
		static URInstructionDocumentSphere* CreateInstructionDocumentSphere(ARobotSphere* DocumentSphere)
		{
			auto Instruction = NewObject<URInstructionDocumentSphere>();
			Instruction->Sphere = DocumentSphere;

			return Instruction;
		}

	void Initialize(ARobot* ParentRobot) override;
	bool ExecuteInstruction(float DeltaTime) override;
};

/**
 * Instruction to drop a sphere.
 */
UCLASS(Blueprintable)
class AIRENGINEERTEST_API URInstructionDropSphere : public URobotInstruction
{
	GENERATED_BODY()

public:
	URInstructionDropSphere();

	UFUNCTION(BlueprintCallable)
		static URInstructionDropSphere* CreateInstructionDropSphere()
		{
			auto Instruction = NewObject<URInstructionDropSphere>();

			return Instruction;
		}

	void Initialize(ARobot* ParentRobot) override;
	bool ExecuteInstruction(float DeltaTime) override;
};