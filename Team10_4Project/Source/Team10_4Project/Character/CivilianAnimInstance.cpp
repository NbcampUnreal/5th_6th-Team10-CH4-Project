// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CivilianAnimInstance.h"

#include "Character/Civilian.h"
#include "Kismet/KismetMathLibrary.h" 
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
}

void UCivilianAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (IsValid(OwnerCharacter) == false || IsValid(OwnerCharacterMovementComponent) == false)
	{
		return;
	}
	
	// 속도 계산
	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
	
	// Pitch 계산
	FRotator AimRotation = OwnerCharacter->GetBaseAimRotation();
	float Pitch = AimRotation.Pitch;
	
	if (Pitch > 180.0f)
	{
		Pitch -= 360.0f;
	}
    
	AimPitch = Pitch; // 최종 계산된 값을 할당
	
	//  Direction (이동 방향) 계산
	if (GroundSpeed > 3.0f) // 움직이고 있을 때만 방향을 계산
	{
		// 현재 보는 방향 (Actor Rotation)
		FRotator ActorRotation = OwnerCharacter->GetActorRotation();
        
		// 실제 이동 방향 (Velocity Direction)
		FRotator MovementRotation = Velocity.ToOrientationRotator();

		// 두 각도의 차이(Delta) (결과값: -180 ~ 180)
		Direction = FMath::FindDeltaAngleDegrees(ActorRotation.Yaw, MovementRotation.Yaw);
	}
	else
	{
		// 멈춰있으면 방향은 0 (혹은 이전 값을 유지)
		Direction = 0.0f;
	}
	
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Speed: %.2f | Falling: %d | Move: %d | Pitch: %.2f | Dir: %.2f"), 
			GroundSpeed, bIsFalling, bShouldMove, AimPitch, Direction);
            
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, DebugMsg);
	}
}

void UCivilianAnimInstance::AnimNotify_CheckMeleeAttackHit()
{
	if (IsValid(OwnerCharacter) == true)
	{
		// OwnerCharacter->CheckMeleeAttackHit();
	}
}
