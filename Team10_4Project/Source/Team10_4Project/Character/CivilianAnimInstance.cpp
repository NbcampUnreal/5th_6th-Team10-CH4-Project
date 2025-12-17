// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CivilianAnimInstance.h"

#include "Character/Civilian.h"
#include "GameFramework/CharacterMovementComponent.h"

UCivilianAnimInstance::UCivilianAnimInstance()
{
	GroundSpeed = 0.0f;
	bIsFalling = false;
	bShouldMove = false;
}

void UCivilianAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwnerCharacter = Cast<ACivilian>(GetOwningActor());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
	
	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();

	// AimPitch = OwnerCharacter->GetCurrentAimPitch();
}

void UCivilianAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UCivilianAnimInstance::AnimNotify_CheckMeleeAttackHit()
{
	if (IsValid(OwnerCharacter) == true)
	{
		// OwnerCharacter->CheckMeleeAttackHit();
	}
}
