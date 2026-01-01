// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "AmmoWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void InitWithASC(UAbilitySystemComponent* ASC);

protected:
    virtual void NativeDestruct() override;

    // 현재 탄창 내 탄약 
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentAmmoText;

    // 보유 중인 전체 여분 탄약 
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ReserveAmmoText;

private:
    FDelegateHandle AmmoChangedHandle;
    FDelegateHandle ReserveAmmoChangedHandle;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CachedASC;

    void OnAmmoAttributeChanged(const FOnAttributeChangeData& Data);
    void UpdateAmmoDisplay(float CurrentAmmo, float ReserveAmmo);
};
