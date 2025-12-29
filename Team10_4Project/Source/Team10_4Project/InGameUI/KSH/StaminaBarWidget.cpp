// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameUI/KSH/StaminaBarWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GameAttributeTypes.h" 

void UStaminaBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UStaminaBarWidget::NativeDestruct()
{
    // Delegate Unbind (UI 파괴될 때 크래시 방지)
    //if (AbilitySystemComponent)
    //{
    //    AbilitySystemComponent
    //        ->GetGameplayAttributeValueChangeDelegate(GET_STAMINA_ATTRIBUTE)
    //        .Remove(StaminaChangedHandle);

    //    AbilitySystemComponent
    //        ->GetGameplayAttributeValueChangeDelegate(GET_MAX_STAMINA_ATTRIBUTE)
    //        .Remove(MaxStaminaChangedHandle);
    //}

    Super::NativeDestruct();
}

void UStaminaBarWidget::InitWithASC(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    AbilitySystemComponent = ASC;

    BindGASDelegates(ASC);

    // 위젯 초기 표시값 설정
    //float CurrentStamina = ASC->GetNumericAttribute(GET_STAMINA_ATTRIBUTE);
    //float MaxStamina = ASC->GetNumericAttribute(GET_MAX_STAMINA_ATTRIBUTE);

    //UpdateStaminaBar(CurrentStamina, MaxStamina);
}

void UStaminaBarWidget::BindGASDelegates(UAbilitySystemComponent* ASC)
{
    // Stamina 변경 감지
    //StaminaChangedHandle =
    //    ASC->GetGameplayAttributeValueChangeDelegate(GET_STAMINA_ATTRIBUTE)
    //    .AddUObject(this, &UStaminaBarWidget::OnStaminaAttributeChanged);

    // MaxStamina 변경 감지
    //MaxStaminaChangedHandle =
    //    ASC->GetGameplayAttributeValueChangeDelegate(GET_MAX_STAMINA_ATTRIBUTE)
    //    .AddUObject(this, &UStaminaBarWidget::OnMaxStaminaAttributeChanged);
}

void UStaminaBarWidget::OnStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!AbilitySystemComponent) return;

    //float MaxStamina = AbilitySystemComponent
    //    ->GetNumericAttribute(GET_MAX_STAMINA_ATTRIBUTE);

    //UpdateStaminaBar(Data.NewValue, MaxStamina);
}

void UStaminaBarWidget::OnMaxStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
    //if (!AbilitySystemComponent) return;

    //float CurrentStamina = AbilitySystemComponent
    //    ->GetNumericAttribute(GET_STAMINA_ATTRIBUTE);

    //UpdateStaminaBar(CurrentStamina, Data.NewValue);
}

void UStaminaBarWidget::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
    if (StaminaBar && MaxStamina > 0.f)
    {
        const float Percent = CurrentStamina / MaxStamina;
        StaminaBar->SetPercent(Percent);
    }
}
