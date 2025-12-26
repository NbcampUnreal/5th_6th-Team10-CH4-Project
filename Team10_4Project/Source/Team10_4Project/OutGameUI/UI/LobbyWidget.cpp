#include "LobbyWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "OutGameUI/UI/LobbyWidget.h"
#include "OutGameUI/UI/PlayerListWidget.h"

void ULobbyWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (LeaveButton)
    {
        LeaveButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClick_Leave);
    }

    UE_LOG(LogTemp, Log, TEXT("LobbyWidget Initialized"));

    if (PlayerListWidget)
    {
        PlayerListWidget->ClearPlayers();

        PlayerListWidget->AddPlayer(TEXT("HostPlayer"));
        PlayerListWidget->AddPlayer(TEXT("Client_1"));
        PlayerListWidget->AddPlayer(TEXT("Client_2"));
    }
}

void ULobbyWidget::OnClick_Leave()
{
    UE_LOG(LogTemp, Log, TEXT("Leave Lobby Clicked"));

    // 일단 테스트 단계이므로 메인메뉴 레벨로 이동
    UGameplayStatics::OpenLevel(this, FName("L_UITest"));
}
