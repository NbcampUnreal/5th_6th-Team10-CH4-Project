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

void UPlayerListWidget::AddPlayer(const FString& PlayerName)
{
    if (!PlayerListScrollBox || !PlayerEntryWidgetClass)
        return;

    UUserWidget* EntryWidget = CreateWidget<UUserWidget>(
        GetWorld(),
        PlayerEntryWidgetClass
    );

    if (!EntryWidget)
        return;

    UTextBlock* NameText =
        Cast<UTextBlock>(EntryWidget->GetWidgetFromName(TEXT("PlayerNameText")));

    if (NameText)
    {
        NameText->SetText(FText::FromString(PlayerName));
    }

    PlayerListScrollBox->AddChild(EntryWidget);
}
