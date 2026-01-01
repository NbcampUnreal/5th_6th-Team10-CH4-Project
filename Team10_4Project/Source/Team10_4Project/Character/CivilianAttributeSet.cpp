// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CivilianAttributeSet.h"

#include "Character/Civilian.h"
#include "GamePlayTag/GamePlayTags.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Character/CivilianPlayerState.h"

UCivilianAttributeSet::UCivilianAttributeSet()
{
	// 초기값 설정
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitSanity(0.0f);
	InitMoveSpeed(300.0f);
	InitCurrentAmmo(10.0f);
	InitMaxClipAmmo(30.0f);
	InitAmmo(15.0f);
	InitMaxAmmo(100.0f);
}

void UCivilianAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCivilianAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCivilianAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCivilianAttributeSet, Sanity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCivilianAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCivilianAttributeSet, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCivilianAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);
}

void UCivilianAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// 체력은 0~MaxHealth 범위로 제한
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// 감염도(혈액팩 게이지) 0~100 범위로 제한
	if (Attribute == GetSanityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}

	// 여분 탄약 0~100 범위로 제한
	if (Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmo());
	}

	// [추가] 현재 탄약(CurrentAmmo)은 MaxClipAmmo(탄창 크기)를 넘을 수 없음
	if (Attribute == GetCurrentAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxClipAmmo());
	}
}

void UCivilianAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 데미지 처리
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		SetDamage(0.0f); // 데미지 소비

		if (LocalDamage > 0.0f)
		{
			const float OldHealth = GetHealth();
			const float NewHealth = OldHealth - LocalDamage;
            
			// 체력 적용 (일단 0 밑으로 안 내려가게)
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

			// [핵심 로직] 이번 공격으로 체력이 0이 되었는가?
			if (OldHealth > 0.0f && NewHealth <= 0.0f)
			{
				// 공격자(Source) 정보 가져오기
				AActor* SourceActor = Data.EffectSpec.GetContext().GetInstigator();
				UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();

				// 피격자(Target/Me) 정보 가져오기
				ACivilian* TargetCharacter = Cast<ACivilian>(Data.Target.GetAvatarActor());
				
				if (TargetCharacter && SourceASC)
				{
					// 공격자가 '변신 상태(Transformed)'인지 확인
					bool bAttackerIsTransformed = SourceASC->HasMatchingGameplayTag(GamePlayTags::InfectedState::Transformed);

					// 캐릭터에게 "너 치명상 입었어"라고 알림 (공격자 정보 포함)
					TargetCharacter->HandleFatalDamage(SourceActor, bAttackerIsTransformed);
				}
			}
			else
			{
				// 죽지 않았으면 피격 모션 처리 (기존 로직)
				if (ACivilian* TargetCharacter = Cast<ACivilian>(Data.Target.GetAvatarActor()))
				{
					// TargetCharacter->HandleTakeDamage(); // 피격 애니메이션 등
				}

				// 대상이 이미 체력이 0인데 공격 받은 경우
				if (OldHealth <= 0.0f && NewHealth <= 0.0f)
				{
					UAbilitySystemComponent* TargetASC = &Data.Target;
					// 공격받는 대상이 이미 무력화상태 인 경우 (투표가능 상태)
					if (TargetASC->HasMatchingGameplayTag(GamePlayTags::CivilianState::Stun))
					{
						//ACivilian* TargetCharacter = Cast<ACivilian>(TargetASC->GetAvatarActor());
						ACivilianPlayerState* TargetOwner = Cast<ACivilianPlayerState>(TargetASC->GetOwnerActor());
						ACivilianPlayerState* SourceOwner = Cast<ACivilianPlayerState>(Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent()->GetOwnerActor());
						// 투표 ServerRPC 실행
						TargetOwner->ServerRPCTryVote(SourceOwner);
					}
				}
			}
		}
	}
}

void UCivilianAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, Health, OldHealth);
}

void UCivilianAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, MaxHealth, OldMaxHealth);
}

void UCivilianAttributeSet::OnRep_Sanity(const FGameplayAttributeData& OldSanity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, Sanity, OldSanity);
}

void UCivilianAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UCivilianAttributeSet::OnRep_CurrentAmmo(const FGameplayAttributeData& OldCurrentAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, CurrentAmmo, OldCurrentAmmo);
}

void UCivilianAttributeSet::OnRep_MaxClipAmmo(const FGameplayAttributeData& OldMaxClipAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, MaxClipAmmo, OldMaxClipAmmo);
}

void UCivilianAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, Ammo, OldAmmo);
}

void UCivilianAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCivilianAttributeSet, MaxAmmo, OldMaxAmmo);
}