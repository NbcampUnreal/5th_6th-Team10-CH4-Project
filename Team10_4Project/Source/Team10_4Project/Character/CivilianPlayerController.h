// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CivilianPlayerController.generated.h"

class ASpectatorCamera;
/**
 * 
 */
class UChatWidget;
UCLASS()
class TEAM10_4PROJECT_API ACivilianPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACivilianPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ----- ChatWidget
public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_SendChatMessage(const FChatMessage& ChatMessage);
	UFUNCTION(Client, Reliable)
	void ClientRPC_ReceiveMessage(const FChatMessage& ChatMessage);
	// ----------------
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_Spectator(APawn* TargetPawn);

private:
	UPROPERTY()
	ASpectatorCamera* SpectatorCamera;
};
