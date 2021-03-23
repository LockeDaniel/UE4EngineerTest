// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotSphere.h"

// Sets default values
ARobotSphere::ARobotSphere()
{
	RootComponent = SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ARobotSphere::BeginPlay()
{
	Super::BeginPlay();
	
}