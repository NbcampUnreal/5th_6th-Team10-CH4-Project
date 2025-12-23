#include "InGameUI/JKH/ChatWidget.h"
#include "InGameUI/JKH/ChatSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputCommitted) == false)
	{
		ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputCommitted);
	}
	ChatInput->ClearKeyboardFocusOnCommit = false;

	UChatSubsystem* ChatSubsystem = GetGameInstance()->GetSubsystem<UChatSubsystem>();
	if (ChatSubsystem)
	{
		ChatSubsystem->OnChatMessageReceived.AddDynamic(this, &UChatWidget::OnChatMessage);
	}
}

void UChatWidget::NativeDestruct()
{
	if (ChatInput)
	{
		ChatInput->OnTextCommitted.Clear();
	}
	Super::NativeDestruct();
}

void UChatWidget::OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter) return;
	if (Text.IsEmpty())
	{
		GetOwningPlayer()->SetInputMode(FInputModeGameOnly());
		return;
	}
	APlayerState* PlayerState = GetOwningPlayerState();
	if (IsValid(PlayerState) == false) return;
	FString PlayerName = PlayerState->GetPlayerName();
	
	FString Message = TEXT("");
	FChatMessage ChatMessage(EMessageType::Normal, PlayerName, Message);
	OnChatMessage(ChatMessage);

	UChatSubsystem* ChatSubsystem = GetGameInstance()->GetSubsystem<UChatSubsystem>();
	if (IsValid(ChatSubsystem) == false) return;
	ChatSubsystem->SendChatMessage(ChatMessage);

	ChatInput->SetText(FText::GetEmpty());	// 채팅 비우기
	// OnChattingCommitted 함수가 종료되면 Focus 해제 로직이 실행됨
	// 다음 Tick에 실행되도록 설정
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::ChatInputReady);
}

void UChatWidget::ChatInputReady()
{
	ChatInput->SetUserFocus(GetOwningPlayer());
	FSlateApplication::Get().SetKeyboardFocus(ChatInput->TakeWidget());
}

void UChatWidget::OnChatMessage(const FChatMessage& ChatMessage)
{
	FText PrintMessage;
	FLinearColor FontColor(1.0f, 1.0f, 1.0f, 1.0f);	// white
	switch (ChatMessage.MessageType)
	{
	case EMessageType::System:
		PrintMessage = FText::FromString(ChatMessage.Message);
		FontColor = FLinearColor(1.0f, 0.647f, 0.0f, 1.0f);
		break;
	case EMessageType::Normal:
		PrintMessage = FText::FromString(ChatMessage.PlayerName + TEXT(": ") + ChatMessage.Message);
		break;
	}
	UTextBlock* ChatTextBlock = NewObject<UTextBlock>(GetOwningPlayer());
	if (ChatTextBlock == nullptr) return;

	ChatTextBlock->SetText(PrintMessage);
	ChatTextBlock->SetColorAndOpacity(FontColor);
	UVerticalBoxSlot* TextSlot = ChatMessageList->AddChildToVerticalBox(ChatTextBlock);
}