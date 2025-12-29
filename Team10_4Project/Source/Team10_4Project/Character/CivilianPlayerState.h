// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "InGameUI/KSH/InventoryItemData.h" // 인벤토리 컴포넌트
#include "CivilianPlayerState.generated.h"

class UAbilitySystemComponent;
class UCivilianAttributeSet;
class UInventoryComponent; // 인벤토리 컴포넌트

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Civilian UMETA(DisplayName = "Civilian"),
	Infected UMETA(DisplayName = "Infected")
};

UCLASS()
class TEAM10_4PROJECT_API ACivilianPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACivilianPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UCivilianAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 태그 기반 역할 변경 함수 (서버 전용)
	UFUNCTION(BlueprintAuthorityOnly, Category = "GAS|Role")
	void SetPlayerRoleTag(FGameplayTag NewRoleTag);

	// 현재 역할 확인 함수 (태그 검사)
	UFUNCTION(BlueprintCallable, Category = "GAS|Role")
	bool IsPlayerRole(FGameplayTag RoleTag) const;
	
protected:
	// 시민 역할 할당 GE
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Role")
	TSubclassOf<class UGameplayEffect> CivilianRoleEffectClass;

	// 감염자 역할 할당 GE
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Role")
	TSubclassOf<class UGameplayEffect> InfectedRoleEffectClass;

	// 인벤토리 컴포넌트 변수 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UCivilianAttributeSet> AttributeSet;
};
