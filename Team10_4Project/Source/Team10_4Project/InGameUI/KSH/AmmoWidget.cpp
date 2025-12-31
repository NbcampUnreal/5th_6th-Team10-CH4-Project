// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/AmmoWidget.h"
#include "Components/TextBlock.h"
#include "Character/CivilianAttributeSet.h"

void UAmmoWidget::InitWithASC(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;
    CachedASC = ASC;

    // 현재 탄창 탄약 감지 (CurrentAmmo)
    AmmoChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UCivilianAttributeSet::GetCurrentAmmoAttribute()).AddUObject(this, &UAmmoWidget::OnAmmoAttributeChanged);

    // 여분 탄약 감지 (Ammo)
    ReserveAmmoChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UCivilianAttributeSet::GetAmmoAttribute()).AddUObject(this, &UAmmoWidget::OnAmmoAttributeChanged);

    // 초기값 설정
    float Current = ASC->GetNumericAttribute(UCivilianAttributeSet::GetCurrentAmmoAttribute());
    float Reserve = ASC->GetNumericAttribute(UCivilianAttributeSet::GetAmmoAttribute());
    UpdateAmmoDisplay(Current, Reserve);
}

void UAmmoWidget::NativeDestruct()
{
    if (CachedASC)
    {
        // 등록했던 핸들을 사용하여 델리게이트 해제
        CachedASC->GetGameplayAttributeValueChangeDelegate(UCivilianAttributeSet::GetCurrentAmmoAttribute()).Remove(AmmoChangedHandle);
        CachedASC->GetGameplayAttributeValueChangeDelegate(UCivilianAttributeSet::GetAmmoAttribute()).Remove(ReserveAmmoChangedHandle);
    }

    Super::NativeDestruct();
}

void UAmmoWidget::OnAmmoAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!CachedASC) return;

    float Current = CachedASC->GetNumericAttribute(
        UCivilianAttributeSet::GetCurrentAmmoAttribute());

    float Reserve = CachedASC->GetNumericAttribute(
        UCivilianAttributeSet::GetAmmoAttribute());

    UpdateAmmoDisplay(Current, Reserve);
}

void UAmmoWidget::UpdateAmmoDisplay(float Current, float Reserve)
{
    if (CurrentAmmoText)
        CurrentAmmoText->SetText(FText::AsNumber(FMath::FloorToInt(Current)));

    if (ReserveAmmoText)
        ReserveAmmoText->SetText(FText::AsNumber(FMath::FloorToInt(Reserve)));
}