// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h" // FOnAttributeChangeData, FDelegateHandle 사용을 위해 필요
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameAttributeTypes.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // HUD에서 ASC를 전달하여 초기화
    void InitWithASC(UAbilitySystemComponent* ASC);

protected:
    // UMG ProgressBar
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

private:
    // Ability System Component (UI가 직접 찾지 않음, 외부에서 주입됨)
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    // Delegate Handles (unbind 필요)
    FDelegateHandle HealthChangedHandle;
    FDelegateHandle MaxHealthChangedHandle;

    void BindGASDelegates(UAbilitySystemComponent* ASC);

    void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
    void OnMaxHealthAttributeChanged(const FOnAttributeChangeData& Data);
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);
};
