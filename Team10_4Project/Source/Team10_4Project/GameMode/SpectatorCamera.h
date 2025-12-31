// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "SpectatorCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API ASpectatorCamera : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ASpectatorCamera();

	void SetFollowTargetPawn(APawn* TargetPawn);
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	
private:
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY()
	TObjectPtr<APawn> FollowTargetPawn;
};
