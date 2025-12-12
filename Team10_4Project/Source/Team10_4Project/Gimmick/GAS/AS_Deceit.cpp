// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/GAS/AS_Deceit.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

UAS_Deceit::UAS_Deceit() : TransformGauge(0.0f), MaxTransformGauge(100.0f), TrapCount(0.0f)
{

}

void UAS_Deceit::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//  변신 게이지 클램핑: 0과 MaxTransformGauge 사이로 제한
	if (Attribute == GetTransformGaugeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxTransformGauge());
	}

	// 트랩 수 클램핑: 0 미만으로 내려가지 않도록 제한
	else if (Attribute == GetTrapCountAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UAS_Deceit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// TransformGauge가 복제되도록 설정
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_Deceit, TransformGauge, COND_None, REPNOTIFY_Always);
}

void UAS_Deceit::OnRep_TransformGauge(const FGameplayAttributeData& OldTransformGauge)
{
	// Attribute 값이 변경되었음을 ASC에 통지
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_Deceit, TransformGauge, OldTransformGauge);

	// 여기에 클라이언트에서 추가적인 시각 효과(UI 업데이트, 파티클 등)를 처리하는 로직을 추가할 수 있음.
}