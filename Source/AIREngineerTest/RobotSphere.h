// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "RobotSphere.generated.h"

UCLASS()
class AIRENGINEERTEST_API ARobotSphere : public AActor
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------
	// Components
	// -----------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
		UStaticMeshComponent* SphereMesh;
	// -----------------------------------------------------------------

public:	
	ARobotSphere();

protected:
	virtual void BeginPlay() override;

};
