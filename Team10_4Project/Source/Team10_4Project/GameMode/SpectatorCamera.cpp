// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/SpectatorCamera.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASpectatorCamera::ASpectatorCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->TargetArmLength = 100.f;
	SpringArmComponent->bDoCollisionTest = false;

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent);
}

void ASpectatorCamera::SetFollowTargetPawn(APawn* TargetPawn)
{
	if (!TargetPawn)
	{
		return;
	}

	FollowTargetPawn = TargetPawn;
}

void ASpectatorCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Error, TEXT("Spectator"));
	
	if (!FollowTargetPawn)
	{
		return;
	}

	SetActorLocation(FollowTargetPawn->GetActorLocation());
	SetActorRotation(FollowTargetPawn->GetActorRotation());
	
}

void ASpectatorCamera::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	//Super::CalcCamera(DeltaTime, OutResult);

	ThirdPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
}
