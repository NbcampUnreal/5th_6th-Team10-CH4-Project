// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_Deceit.generated.h"


// Attribute 접근자를 자동으로 생성해 주는 매크로 정의
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class TEAM10_4PROJECT_API UAS_Deceit : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAS_Deceit();

#pragma region Transform Gauge (변신 게이지 관련)
	// 현재 변신 게이지
	UPROPERTY(BlueprintReadOnly, Category = "TransformGauge", ReplicatedUsing = OnRep_TransformGauge)
	FGameplayAttributeData TransformGauge;
	ATTRIBUTE_ACCESSORS(UAS_Deceit, TransformGauge)

	// 클라이언트에서 TransformGauge 값이 복제될 때 호출될 함수 선언
	UFUNCTION()
	void OnRep_TransformGauge(const FGameplayAttributeData& OldTransformGauge);

	// 최대 변신 게이지
	UPROPERTY(BlueprintReadOnly, Category = "TransformGauge")
	FGameplayAttributeData MaxTransformGauge;
	ATTRIBUTE_ACCESSORS(UAS_Deceit, MaxTransformGauge)
#pragma endregion


#pragma region Inventory/Resources (인벤토리/자원 관련)
	// 플레이어가 가진 트랩의 수
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	FGameplayAttributeData TrapCount;
	ATTRIBUTE_ACCESSORS(UAS_Deceit, TrapCount)
#pragma endregion


#pragma region PreAttributeChange Logics (Attribute 변경 전 호출될 함수 [클램핑 및 로직 검증])
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
#pragma endregion
};
