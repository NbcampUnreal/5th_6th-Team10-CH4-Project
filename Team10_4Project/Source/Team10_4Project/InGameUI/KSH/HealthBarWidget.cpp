// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GameAttributeTypes.h" 

void UHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHealthBarWidget::NativeDestruct()
{
    // Delegate Unbind (UI 파괴될 때 크래시 방지)
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent
            ->GetGameplayAttributeValueChangeDelegate(GET_HEALTH_ATTRIBUTE)
            .Remove(HealthChangedHandle);

        AbilitySystemComponent
            ->GetGameplayAttributeValueChangeDelegate(GET_MAX_HEALTH_ATTRIBUTE)
            .Remove(MaxHealthChangedHandle);
    }

    Super::NativeDestruct();
}

void UHealthBarWidget::InitWithASC(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    AbilitySystemComponent = ASC;

    BindGASDelegates(ASC);

    // 위젯 초기 표시값 설정
    float CurrentHealth = ASC->GetNumericAttribute(GET_HEALTH_ATTRIBUTE);
    float MaxHealth = ASC->GetNumericAttribute(GET_MAX_HEALTH_ATTRIBUTE);

    UpdateHealthBar(CurrentHealth, MaxHealth);
}

void UHealthBarWidget::BindGASDelegates(UAbilitySystemComponent* ASC)
{
    // Health 변경 감지
    HealthChangedHandle =
        ASC->GetGameplayAttributeValueChangeDelegate(GET_HEALTH_ATTRIBUTE)
        .AddUObject(this, &UHealthBarWidget::OnHealthAttributeChanged);

    // MaxHealth 변경 감지
    MaxHealthChangedHandle =
        ASC->GetGameplayAttributeValueChangeDelegate(GET_MAX_HEALTH_ATTRIBUTE)
        .AddUObject(this, &UHealthBarWidget::OnMaxHealthAttributeChanged);
}

void UHealthBarWidget::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!AbilitySystemComponent) return;

    float MaxHealth = AbilitySystemComponent
        ->GetNumericAttribute(GET_MAX_HEALTH_ATTRIBUTE);

    UpdateHealthBar(Data.NewValue, MaxHealth);
}

void UHealthBarWidget::OnMaxHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!AbilitySystemComponent) return;

    float CurrentHealth = AbilitySystemComponent
        ->GetNumericAttribute(GET_HEALTH_ATTRIBUTE);

    UpdateHealthBar(CurrentHealth, Data.NewValue);
}

void UHealthBarWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (HealthBar && MaxHealth > 0.f)
    {
        const float Percent = CurrentHealth / MaxHealth;
        HealthBar->SetPercent(Percent);
    }
}