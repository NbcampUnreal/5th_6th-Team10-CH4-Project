#include "InGameUI/JKH/ChatSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode/Team10GameMode.h"

UChatSubsystem::UChatSubsystem()
{
}

void UChatSubsystem::SendChatMessage(const FChatMessage& ChatMessage)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (IsValid(PlayerController) == false) return;
	// PlayerController->ServerRPC_SendChatMessage(ChatMessage);
}

void UChatSubsystem::ReceiveChatMessage(const FChatMessage& ChatMessage)
{
	if (OnChatMessageReceived.IsBound())
	{
		OnChatMessageReceived.Broadcast(ChatMessage);
	}
}

void UChatSubsystem::ClearChatHistory()
{
	ChatHistory.Empty();
}

/*
void APlayerController::ServerRPC_SendChatMessage_Implementation(const FChatMessage& ChatMessage)
{
	ATeam10GameMode* Team10GameMode = GetWorld()->GetAuthGameMode<ATeam10GameMode>();
	if (IsValid(Team10GameMode) == false) return;	// not server

	Team10GameMode->ProcessChatMessage(this, ChatMessage);
}

void ATeam10GameMode::ProcessChatMessage(APlayerController* InPlayerController, const FChatMessage& ChatMessage)
{
	//TArray<TObjectPtr<APlayerController>> PlayerList;
	TArray<TWeakObjectPtr<APlayerController>> ValidPlayerList;
	FChatMessage RefinedChatMessage = ChatMessage;

	APlayerState* InPlayerState = InPlayerController->PlayerState;
	if (IsValid(InPlayerState) == false) return;
	FString SenderName = InPlayerState->GetPlayerName();

	for (APlayerController* ClientPlayer : PlayerList)
	{
		if (IsValid(ClientPlayer) == false) continue;
		if (ClientPlayer == InPlayerController)
		{
			if (RefinedChatMessage.PlayerName != SenderName)	// hacking ?
			{
				RefinedChatMessage.PlayerName = SenderName;
			}
			continue;
		}
		ValidPlayerList.Push(ClientPlayer);
	}
	
	if (ValidPlayerList.Num() <= 0) return;

	for (APlayerController* ValidPlayer : ValidPlayerList)
	{
		ValidPlayer->ClientRPC_ReceiveMessage(RefinedChatMessage);
	}
}

void APlayerController::ClientRPC_ReceiveMessage_Implementation(const FChatMessage& ChatMessage)
{
	UChatSubsystem* ChatSubSystem = GetGameInstance()->GetSubsystem<UChatSubsystem>();
	if (IsValid(ChatSubSystem) == false) return;
	ChatSubSystem->ReceiveChatMessage(ChatMessage);
}
*/