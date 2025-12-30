#include "OutGameUI/UI/PlayerListWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

void UPlayerListWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UPlayerListWidget::ClearPlayers()
{
    if (PlayerListScrollBox)
    {
        PlayerListScrollBox->ClearChildren();
    }
}

void UPlayerListWidget::AddPlayer(const FString& PlayerName, bool bIsReady)
{
    if (!PlayerListScrollBox || !PlayerEntryWidgetClass)
        return;

    UUserWidget* EntryWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerEntryWidgetClass);

    if (!EntryWidget)
        return;

    // 1. 이름 설정
    UTextBlock* NameText = Cast<UTextBlock>(EntryWidget->GetWidgetFromName(TEXT("PlayerNameText")));
    if (NameText)
    {
        NameText->SetText(FText::FromString(PlayerName));
    }

    UTextBlock* ReadyText = Cast<UTextBlock>(EntryWidget->GetWidgetFromName(TEXT("ReadyText")));
    if (ReadyText)
    {
        FText Status = bIsReady ? FText::FromString(TEXT("READY")) : FText::FromString(TEXT("NOT READY"));
        ReadyText->SetText(Status);

        // 시각적 피드백: 준비 완료 시 초록색
        ReadyText->SetColorAndOpacity(bIsReady ? FLinearColor::Green : FLinearColor::White);
    }

    PlayerListScrollBox->AddChild(EntryWidget);
}