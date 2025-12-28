// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CivilianPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Character/CivilianAttributeSet.h"
#include "GamePlayTag/GamePlayTags.h"
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
	
	DOREPLIFETIME(ThisClass, VoterList);	// Vote
}

void ACivilianPlayerState::SetPlayerRoleTag(FGameplayTag NewRoleTag)
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	TSubclassOf<UGameplayEffect> EffectToApply = nullptr;
	
	// 태그에 따라 적용할 GE 결정
	if (NewRoleTag == GamePlayTags::PlayerRole::Civilian)
	{
		EffectToApply = CivilianRoleEffectClass;
	}
	else if (NewRoleTag == GamePlayTags::PlayerRole::Infected)
	{
		EffectToApply = InfectedRoleEffectClass;
	}

	// GE 적용
	if (EffectToApply)
	{
		// Effect Context 생성 (누가 적용했는지 등 정보 포함)
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		// GE 적용 
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			EffectToApply.GetDefaultObject(),
			1.0f,
			ContextHandle
		);
        
		UE_LOG(LogTemp, Log, TEXT("Role Changed to: %s"), *NewRoleTag.ToString());
	}
}

bool ACivilianPlayerState::IsPlayerRole(FGameplayTag RoleTag) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(RoleTag);
	}
	return false;
}

void ACivilianPlayerState::OnRep_Voters()
{
	if (OnVoterListChanged.IsBound())
	{
		OnVoterListChanged.Broadcast();
	}
}
