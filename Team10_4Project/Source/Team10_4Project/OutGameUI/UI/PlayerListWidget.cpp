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
    if (!PlayerListScrollBox || !PlayerEntryWidgetClass) return;

    UUserWidget* EntryWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerEntryWidgetClass);
    if (!EntryWidget) return;

    if (UTextBlock* NameText = Cast<UTextBlock>(EntryWidget->GetWidgetFromName(TEXT("PlayerNameText"))))
    {
        NameText->SetText(FText::FromString(PlayerName));
    }

    if (UTextBlock* ReadyText = Cast<UTextBlock>(EntryWidget->GetWidgetFromName(TEXT("ReadyText"))))
    {
        ReadyText->SetText(bIsReady ? FText::FromString(TEXT("READY")) : FText::FromString(TEXT("NOT READY")));
        ReadyText->SetColorAndOpacity(bIsReady ? FLinearColor::Green : FLinearColor::White);
    }

    PlayerListScrollBox->AddChild(EntryWidget);
}