#include "InGameUI/JKH/ChatSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode/Team10GameMode.h"
#include "Character/CivilianPlayerController.h"
#include "InGameUI/JKH/ChatWidget.h"

UChatSubsystem::UChatSubsystem()
{
	static ConstructorHelpers::FClassFinder<UChatWidget> BP_ChatWidget(TEXT("/Game/Team10/InGameUI/JKH/WBP_ChatWidget.WBP_ChatWidget_C"));
	if (BP_ChatWidget.Succeeded())
	{
		ChatWidgetClass = BP_ChatWidget.Class;
	}
	else
	{
		ChatWidgetClass = UChatWidget::StaticClass();
	}
	ChatWidgetInstance = nullptr;
}

void UChatSubsystem::OpenWidget(APlayerController* InPlayerController)
{
	if (IsValid(ChatWidgetInstance))
	{
		CloseWidget();
	}
	if (IsValid(ChatWidgetClass))
	{
		ChatWidgetInstance = CreateWidget<UChatWidget>(InPlayerController, ChatWidgetClass);
		ChatWidgetInstance->AddToViewport();
	}
}

void UChatSubsystem::CloseWidget()
{
	if (IsValid(ChatWidgetInstance))
	{
		ChatWidgetInstance->RemoveFromParent();
		ChatWidgetInstance = nullptr;
	}
}

void UChatSubsystem::SendChatMessage(const FChatMessage& ChatMessage)
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ACivilianPlayerController* PlayerController = World->GetFirstPlayerController<ACivilianPlayerController>();
	if (!IsValid(PlayerController)) return;
	PlayerController->ServerRPC_SendChatMessage(ChatMessage);
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

void UChatSubsystem::OnChatCommit()
{
	if (!IsValid(ChatWidgetInstance)) return;
	ChatWidgetInstance->ChatInputReady();
}


