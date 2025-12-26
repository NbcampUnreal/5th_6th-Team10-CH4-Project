// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "GameTypes/GameTypes.h"
#include "PlayerSpawn.generated.h"



UCLASS()
class TEAM10_4PROJECT_API APlayerSpawn : public APlayerStart
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	//FGameplayTag AreaTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FGameplayTag AreaTag;
};
