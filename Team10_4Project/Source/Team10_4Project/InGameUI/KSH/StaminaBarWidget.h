// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h" // FOnAttributeChangeData, FDelegateHandle 사용을 위해 필요
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameAttributeTypes.h"
#include "StaminaBarWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UStaminaBarWidget : public UUserWidget
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
    TObjectPtr<UProgressBar> StaminaBar;

private:
    // Ability System Component (UI가 직접 찾지 않음, 외부에서 주입됨)
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    // Delegate Handles (unbind 필요)
    FDelegateHandle StaminaChangedHandle;
    FDelegateHandle MaxStaminaChangedHandle;

    void BindGASDelegates(UAbilitySystemComponent* ASC);

    void OnStaminaAttributeChanged(const FOnAttributeChangeData& Data);
    void OnMaxStaminaAttributeChanged(const FOnAttributeChangeData& Data);
    void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

};
