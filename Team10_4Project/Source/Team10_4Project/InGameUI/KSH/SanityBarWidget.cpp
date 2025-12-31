// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/SanityBarWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GameAttributeTypes.h" 
#include "GamePlayTag/GamePlayTags.h" // GamePlayTags

void USanityBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void USanityBarWidget::NativeDestruct()
{
    // Delegate Unbind (UI 파괴될 때 크래시 방지)
    if (AbilitySystemComponent)
    {
        // Attribute 변경 델리게이트 해제
        AbilitySystemComponent
            ->GetGameplayAttributeValueChangeDelegate(GET_SANITY_ATTRIBUTE)
            .Remove(SanityChangedHandle);

        // 태그 변경 델리게이트 해제
        AbilitySystemComponent
            ->RegisterGameplayTagEvent(GamePlayTags::PlayerRole::Infected, EGameplayTagEventType::NewOrRemoved)
            .Remove(RoleTagHandle);
    }

    Super::NativeDestruct();
}

void USanityBarWidget::InitWithASC(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    AbilitySystemComponent = ASC;
    BindGASDelegates(ASC);

    // 역할 태그 확인 및 핸들 저장
    RoleTagHandle = ASC->RegisterGameplayTagEvent(
        GamePlayTags::PlayerRole::Infected,
        EGameplayTagEventType::NewOrRemoved
    ).AddUObject(this, &USanityBarWidget::OnRoleTagChanged);
    // 현재 역할에 따라 초기상태 설정
    int32 RoleTagCount = ASC->GetTagCount(GamePlayTags::PlayerRole::Infected);
    OnRoleTagChanged(GamePlayTags::PlayerRole::Infected, RoleTagCount);

    // 위젯 초기 표시값 설정
    float CurrentSanity = ASC->GetNumericAttribute(GET_SANITY_ATTRIBUTE);
    UpdateSanityBar(CurrentSanity);
}

void USanityBarWidget::BindGASDelegates(UAbilitySystemComponent* ASC)
{
    UE_LOG(LogTemp, Warning, TEXT("Binding Sanity Delegate Success!"));
    // Sanity 변경 감지
    SanityChangedHandle =
        ASC->GetGameplayAttributeValueChangeDelegate(GET_SANITY_ATTRIBUTE)
        .AddUObject(this, &USanityBarWidget::OnSanityAttributeChanged);
}

void USanityBarWidget::OnSanityAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!AbilitySystemComponent) return;

    UpdateSanityBar(Data.NewValue);
}

void USanityBarWidget::UpdateSanityBar(float CurrentSanity)
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateSanityBar!"));

    if (SanityBar)
    {
        // Sanity는 100.0f를 기준으로 비율 계산
        float SanityPercent = FMath::Clamp(CurrentSanity / 100.0f, 0.0f, 1.0f);
        UE_LOG(LogTemp, Warning, TEXT("UpdateSanityBar! Value: %f, Percent: %f"), CurrentSanity, SanityPercent);
        SanityBar->SetPercent(SanityPercent);
    }
}

void USanityBarWidget::OnRoleTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    UE_LOG(LogTemp, Warning, TEXT("UI Callback: Tag %s changed, NewCount: %d"), *Tag.ToString(), NewCount);
    UpdateVisibilityByInfectedCount(NewCount);
}

void USanityBarWidget::UpdateVisibilityByInfectedCount(int32 Count)
{
    // Infected 태그가 1개 이상이면 보이게 함
    SetVisibility(Count > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}