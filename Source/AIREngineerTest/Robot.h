// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Robot.generated.h"

class URobotInstruction;
class ARobotSphere;

UCLASS()
class AIRENGINEERTEST_API ARobot : public APawn
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------
	// Components
	// -----------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		UStaticMeshComponent* RobotMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		USceneCaptureComponent2D* SceneCapture;
	// -----------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Robot")
		float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		float RotateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		float SphereDocumentDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		float SphereDocumentDistanceMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		float WallCheckDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		float WallCheckWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		bool bWallCheckDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		TSubclassOf<ARobotSphere> SphereType;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Robot")
		UTextureRenderTarget2D* RenderTarget;

	bool bLastWallBounceClockWise;
	int ScreenshotCounter;

protected:
	UPROPERTY()
		TArray<URobotInstruction*> Instructions;

public:	
	ARobot();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Robot")
		void AddInstruction(URobotInstruction* Instruction);

	UFUNCTION(BlueprintCallable, Category = "Robot")
		void ClearInstructions();

	UFUNCTION(BlueprintCallable, Category = "Robot")
		URobotInstruction* GetCurrentInstruction();

	UFUNCTION(BlueprintCallable, Category = "Robot")
		bool HasInstruction()
		{
			return Instructions.Num() > 0;
		}

	UFUNCTION(BlueprintCallable, Category = "Robot")
		float GetLastWallRotationDirection()
		{
			return bLastWallBounceClockWise ? 1.0f : -1.0f;
		}

	UFUNCTION(BlueprintImplementableEvent, Category = "Robot")
		void InstructionsAreEmpty();

protected:
	virtual void BeginPlay() override;

};
