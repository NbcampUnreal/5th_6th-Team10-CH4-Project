// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CivilianPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Character/CivilianAttributeSet.h"
#include "Net/UnrealNetwork.h"

ACivilianPlayerState::ACivilianPlayerState()
{
	SetNetUpdateFrequency(100.0f);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); // ASC 상태 복제
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // ReplicationMode 설정
	
	AttributeSet = CreateDefaultSubobject<UCivilianAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACivilianPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACivilianPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACivilianPlayerState, CurrentRole);
}

void ACivilianPlayerState::SetPlayerRole(EPlayerRole NewRole)
{
	if (HasAuthority())
	{
		CurrentRole = NewRole;
	}
}
