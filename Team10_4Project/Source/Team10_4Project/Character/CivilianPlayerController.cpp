// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CivilianPlayerController.h"

#include "GameMode/SpectatorCamera.h"
#include "GameMode/Team10GameMode.h"
#include "InGameUI/JKH/ChatSubsystem.h"
#include "Team10_4Project/InGameUI/JKH/ChatWidget.h"

ACivilianPlayerController::ACivilianPlayerController()
{
}

void ACivilianPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance) && IsLocalPlayerController())
	{
		UChatSubsystem* ChatSubsystem = GameInstance->GetSubsystem<UChatSubsystem>();
		if (IsValid(ChatSubsystem))
		{
			ChatSubsystem->OpenWidget(this);
		}
	}
}

void ACivilianPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		UChatSubsystem* ChatSubsystem = GameInstance->GetSubsystem<UChatSubsystem>();
		if (IsValid(ChatSubsystem))
		{
			ChatSubsystem->CloseWidget();
		}
	}
	Super::EndPlay(EndPlayReason);
}


void ACivilianPlayerController::ServerRPC_SendChatMessage_Implementation(const FChatMessage& ChatMessage)
{
	ATeam10GameMode* Team10GameMode = GetWorld()->GetAuthGameMode<ATeam10GameMode>();
	if (!IsValid(Team10GameMode)) return;	// not server
	Team10GameMode->ProcessChatMessage(this, ChatMessage);
}

void ACivilianPlayerController::ClientRPC_ReceiveMessage_Implementation(const FChatMessage& ChatMessage)
{
	UChatSubsystem* ChatSubSystem = GetGameInstance()->GetSubsystem<UChatSubsystem>();
	if (IsValid(ChatSubSystem) == false) return;
	ChatSubSystem->ReceiveChatMessage(ChatMessage);
}

void ACivilianPlayerController::ClientRPC_Spectator_Implementation(APawn* TargetPawn)
{
	ChangeState(NAME_Spectating);
	
	if (!GetSpectatorPawn())
	{
		SpawnSpectatorPawn();

		UE_LOG(LogTemp, Error, TEXT("SpawnSpectatorPawn"));
	}

	if (GetSpectatorPawn())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnSpectatorPawn is valid"));
	}
	
	SpectatorCamera = Cast<ASpectatorCamera>(GetSpectatorPawn());
	
	if (SpectatorCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Spectator Camera is valid"));
		
		SpectatorCamera->SetFollowTargetPawn(TargetPawn);
		GetWorldTimerManager().SetTimerForNextTick([this]()
		{
			SetViewTarget(SpectatorCamera);
		});
	
	}
	
}
