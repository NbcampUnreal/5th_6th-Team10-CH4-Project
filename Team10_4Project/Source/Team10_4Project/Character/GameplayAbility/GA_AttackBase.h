// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AttackBase.generated.h"

/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UGA_AttackBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AttackBase();

	// --- [Functions] 블루프린트에서 호출할 기능들 ---

	/**
	 * 타겟을 감지하고 결과를 반환하는 함수 (LineTrace)
	 * @return 맞은 액터가 있다면 HitResult 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	FHitResult PerformLineTrace();

	/**
	 * 범위 공격 판정 (감염자용, SphereTrace)
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool GetEnemiesInRadius(float Radius, TArray<FHitResult>& OutHits);

	/**
	 * 실제로 데미지(GE)를 타겟에게 적용하는 함수
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ApplyDamageToTarget(AActor* TargetActor);

protected:
	// --- [Settings] 에디터에서 설정할 변수들 ---

	// 재생할 공격 애니메이션 몽타주
	// 3인칭용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage3P;

	// 1인칭용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage1P;

	// 공격 사거리 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 1000.0f;

	// 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackDamage = 10.0f;

	// 데미지를 줄 때 적용할 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
