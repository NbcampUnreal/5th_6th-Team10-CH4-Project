#include "ServerSlotWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "OutGameUI/Player/MenuPlayerController.h"

void UServerSlotWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &UServerSlotWidget::OnClick_Join);
    }
}

void UServerSlotWidget::SetServerInfo(
    const FString& InServerName,
    int32 InCurrentPlayers,
    int32 InMaxPlayers,
    int32 InPing
)
{
    if (ServerNameText)
        ServerNameText->SetText(FText::FromString(InServerName));

    if (PlayerCountText)
    {
        const FString PlayerText =
            FString::Printf(TEXT("%d / %d"), InCurrentPlayers, InMaxPlayers);
        PlayerCountText->SetText(FText::FromString(PlayerText));
    }

    if (PingText)
    {
        PingText->SetText(
            FText::FromString(FString::Printf(TEXT("%d ms"), InPing))
        );
    }
}

void UServerSlotWidget::OnClick_Join()
{
    if (AMenuPlayerController* MenuPC =
        Cast<AMenuPlayerController>(GetOwningPlayer()))
    {
        MenuPC->ShowLobby();
    }
}