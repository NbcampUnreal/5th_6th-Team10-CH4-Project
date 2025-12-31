// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Morph.generated.h"

/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UGA_Morph : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Morph();
	
	UFUNCTION(BlueprintCallable, Category = "GAS|Morph")
	void SetCharacterMorphState(bool bToInfected);

protected:
	// 변신 시 재생할 사운드 큐
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> MorphSound;
};
