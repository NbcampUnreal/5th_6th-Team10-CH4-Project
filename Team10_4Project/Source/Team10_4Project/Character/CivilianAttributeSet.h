// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CivilianAttributeSet.generated.h"

// ===== Attribute 매크로 =====
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TEAM10_4PROJECT_API UCivilianAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCivilianAttributeSet();
	
	// 네트워크 복제
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Attribute 값 변경 전 처리
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// GameplayEffect 적용 후 처리
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	// ===== Attribute =====
	
	// Health (체력)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, Health);

	// MaxHealth (최대 체력)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, MaxHealth);
	
	// Damage (데미지) - 임시 처리용
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, Damage);
	
	// Sanity (감염도)
	// (감염자 혈액팩 변신 게이지)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Sanity)
	FGameplayAttributeData Sanity;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, Sanity);
	
	// Move Speed (이동 속도)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Ammo)
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, Ammo)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Ammo)
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UCivilianAttributeSet, MaxAmmo)
	
protected:
	// ===== OnRep Functions (클라이언트 동기롸) =====
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Sanity(const FGameplayAttributeData& OldSanity);
	
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	void OnRep_Ammo(const FGameplayAttributeData& OldAmmo);

	UFUNCTION()
	void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo);
};
