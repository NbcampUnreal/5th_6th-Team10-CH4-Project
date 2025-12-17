// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CivilianAnimInstance.generated.h"

class ACivilian;
class UCharacterMovementComponent;

UCLASS()
class TEAM10_4PROJECT_API UCivilianAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCivilianAnimInstance();
	
	// 애니메이션 초기화
	virtual void NativeInitializeAnimation() override;

	// 매 프레임 갱신
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION()
	void AnimNotify_CheckMeleeAttackHit();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TObjectPtr<ACivilian> OwnerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;
	
	// 이동속도 Idle/Walk 블렌딩용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;
	
	// 움직임 여부 확인 (키 입력중인지)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Movement")
	uint8 bShouldMove : 1;
	
	// 점프 모션 확인용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bIsFalling : 1;

	// 1인칭 무기 흔들림(Sway) 계산용 변수 
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float WeaponSwayX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AimPitch;
};
