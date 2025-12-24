// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GameplayAbility/GA_Morph.h"

#include "Character/Civilian.h"
#include "GamePlayTag/GamePlayTags.h"

UGA_Morph::UGA_Morph()
{
	// 1. 태그 설정
	AbilityTags.AddTag(GamePlayTags::Ability::Infected::Morph);
	ActivationRequiredTags.AddTag(GamePlayTags::PlayerRole::Infected);

	// 2. 중요: 변신은 서버 권한이 필수이므로 ServerOnly로 설정
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Morph::SetCharacterMorphState(bool bToInfected)
{
	if (ACivilian* Civilian = Cast<ACivilian>(CurrentActorInfo->AvatarActor.Get()))
	{
		// 수정된 캐릭터 함수 호출
		Civilian->MulticastMorph(bToInfected);
	}
}
