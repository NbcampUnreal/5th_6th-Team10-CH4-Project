// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Team10GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UTeam10GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	void SaveNickName(FString InNickName);
	
	FString NickName;
};
