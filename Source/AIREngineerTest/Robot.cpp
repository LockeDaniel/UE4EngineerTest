// Fill out your copyright notice in the Description page of Project Settings.


#include "Robot.h"
#include "RobotInstruction.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
ARobot::ARobot()
{
	RootComponent = RobotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RobotMesh"));
	//RobotMesh->SetSimulatePhysics(true);
	//RobotMesh->SetConstraintMode(EDOFMode::XYPlane);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RobotMesh);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(Camera);
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	MoveSpeed = 100.0f;
	RotateSpeed = 90.0f;
	SphereDocumentDistance = 50.0f;
	SphereDocumentDistanceMax = 200.0f;
	WallCheckDistance = 140.0f;
	WallCheckWidth = 120.0f;
	bWallCheckDebug = false;
	bLastWallBounceClockWise = true;
	ScreenshotCounter = 0;

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();

	// Setup render target
	int ResX;
	int ResY;
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ResX, ResY);
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, ResX, ResY, RTF_RGBA8);
	SceneCapture->TextureTarget = RenderTarget;
	RenderTarget->TargetGamma = 2.5f;
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (auto Instruction = GetCurrentInstruction())
	{
		if (!Instruction->IsInitialized())
			Instruction->Initialize(this);

		if (Instruction->ExecuteInstruction(DeltaTime))
		{
			if (Instructions.Contains(Instruction))
				Instructions.Remove(Instruction);
		}
	}

	if (!HasInstruction())
	{
		InstructionsAreEmpty();
	}
}

void ARobot::AddInstruction(URobotInstruction* Instruction)
{
	Instructions.Add(Instruction);
}

void ARobot::ClearInstructions()
{
	Instructions.Empty();
}

URobotInstruction* ARobot::GetCurrentInstruction()
{
	if (Instructions.Num() > 0)
	{
		return Instructions[0];
	}

	return nullptr;
}

