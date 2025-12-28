// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "CivilianPlayerState.generated.h"

class UAbilitySystemComponent;
class UCivilianAttributeSet;

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Civilian UMETA(DisplayName = "Civilian"),
	Infected UMETA(DisplayName = "Infected")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoterListChanged);

UCLASS()
class TEAM10_4PROJECT_API ACivilianPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACivilianPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UCivilianAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 태그 기반 역할 변경 함수 (서버 전용)
	UFUNCTION(BlueprintAuthorityOnly, Category = "GAS|Role")
	void SetPlayerRoleTag(FGameplayTag NewRoleTag);

	// 현재 역할 확인 함수 (태그 검사)
	UFUNCTION(BlueprintCallable, Category = "GAS|Role")
	bool IsPlayerRole(FGameplayTag RoleTag) const;
	
protected:
	// 시민 역할 할당 GE
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Role")
	TSubclassOf<class UGameplayEffect> CivilianRoleEffectClass;

	// 감염자 역할 할당 GE
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Role")
	TSubclassOf<class UGameplayEffect> InfectedRoleEffectClass;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UCivilianAttributeSet> AttributeSet;

#pragma region Vote
public:
	FOnVoterListChanged OnVoterListChanged;
	UFUNCTION()
	void OnRep_Voters();
	TArray<TObjectPtr<APlayerState>> GetVoterList() const { return VoterList; };
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Voters)
	TArray<TObjectPtr<APlayerState>> VoterList;
#pragma endregion
};
