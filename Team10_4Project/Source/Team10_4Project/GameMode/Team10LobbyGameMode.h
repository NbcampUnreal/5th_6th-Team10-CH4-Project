// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Team10LobbyGameMode.generated.h"

class ATeam10GameState;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API ATeam10LobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATeam10LobbyGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameFlow")
	int32 MinPlayersToStart = 6;
	
	UPROPERTY()
	TObjectPtr<ATeam10GameState> Team10GameState;
};
