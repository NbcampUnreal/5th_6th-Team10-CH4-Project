#include "LobbyWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/UI/LobbyWidget.h"
#include "OutGameUI/UI/PlayerListWidget.h"
#include "OutGameUI/Player/MenuPlayerController.h"
#include "OutGameUI/Player/MenuPlayerState.h"

#include "Components/Button.h"

void ULobbyWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (LeaveButton)
    {
        LeaveButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClick_Leave);
    }

    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnReadyButtonClicked);
    }

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartButtonClicked);

        // 방장(Host)이 아닌 경우 버튼을 숨기거나 비활성화
        APlayerController* PC = GetOwningPlayer();
        if (PC && !PC->HasAuthority())
        {
            StartButton->SetVisibility(ESlateVisibility::Collapsed);
        }
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
    if (auto* PC = Cast<AMenuPlayerController>(GetOwningPlayer()))
    {
        PC->ShowMainMenu();
    }
}

void ULobbyWidget::OnReadyButtonClicked()
{
    AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(GetOwningPlayer());

    if (MenuPC && MenuPC->PlayerState)
    {
        FString PlayerName = MenuPC->PlayerState->GetPlayerName();

        UE_LOG(LogTemp, Log, TEXT("[%s] Ready Button Clicked!"), *PlayerName);

        MenuPC->RequestToggleReady();
    }
}

void ULobbyWidget::OnStartButtonClicked()
{
    AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(GetOwningPlayer());
    if (MenuPC)
    {
        // 서버에게 게임 시작을 요청
        MenuPC->RequestStartGame();
    }
}

void ULobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        // GetOwningPlayer()가 서버 권한을 가졌는지 확인
        APlayerController* PC = GetOwningPlayer();
        if (PC && PC->HasAuthority())
        {
            // 방장이라면 버튼을 보여줌
            StartButton->SetVisibility(ESlateVisibility::Visible);
            UE_LOG(LogTemp, Log, TEXT("I am Host: Showing Start Button"));
        }
        else
        {
            // 클라이언트라면 버튼을 완전히 숨김
            StartButton->SetVisibility(ESlateVisibility::Collapsed);
            UE_LOG(LogTemp, Log, TEXT("I am Client: Hiding Start Button"));
        }
    }
}
