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

UCLASS()
class TEAM10_4PROJECT_API ACivilianPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACivilianPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UCivilianAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category = "GameRole")
	EPlayerRole GetPlayerRole() const { return CurrentRole; }
	
	UFUNCTION(BlueprintAuthorityOnly, Category = "GameRole")
	void SetPlayerRole(EPlayerRole NewRole);
	
private:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UCivilianAttributeSet> AttributeSet;
	
	UPROPERTY(replicated, VisibleAnywhere, Category = "GAS")
	EPlayerRole CurrentRole;
};
