#include "OutGameUI/Player/MenuPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "OutGameUI/UI/MainMenuWidget.h"
#include "OutGameUI/UI/ServerBrowserWidget.h"
#include "OutGameUI/UI/LobbyWidget.h"

#include "MenuPlayerController.h"
#include "MenuPlayerState.h"

#include "OutGameUI/GameMode/MenuGameMode.h"

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController())
    {
        bShowMouseCursor = true;
        SetInputMode(FInputModeUIOnly());
        ShowMainMenu();
    }
}
void AMenuPlayerController::ClearCurrentWidget()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }
}

void AMenuPlayerController::ShowMainMenu()
{
    ClearCurrentWidget();

    if (!MainMenuWidgetClass) return;

    CurrentWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();
    }
}

void AMenuPlayerController::ShowServerBrowser()
{
    ClearCurrentWidget();

    if (!ServerBrowserWidgetClass) return;

    CurrentWidget = CreateWidget<UServerBrowserWidget>(this, ServerBrowserWidgetClass);
    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();
    }
}

void AMenuPlayerController::ShowLobby()
{
    ClearCurrentWidget();

    if (!LobbyWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyWidgetClass is NOT assigned in Blueprint!"));
        return;
    }

    ULobbyWidget* LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
    if (LobbyWidget)
    {
        CurrentWidget = LobbyWidget; // 변수에 반드시 저장
        CurrentWidget->AddToViewport();
        UE_LOG(LogTemp, Log, TEXT("LobbyWidget Created and Assigned to CurrentWidget"));
    }
}

void AMenuPlayerController::RequestToggleReady()
{
    Server_ToggleReady();
}

void AMenuPlayerController::Server_ToggleReady_Implementation()
{
    AMenuPlayerState* PS = GetPlayerState<AMenuPlayerState>();
    if (PS)
    {
        bool bNewReadyStatus = !PS->IsReady();
        PS->SetReady(bNewReadyStatus); // 여기서 PlayerState의 OnRep_Ready가 실행됨

        // [추가] 서버(방장) 본인의 화면도 즉시 업데이트하기 위해 호출
        if (IsLocalController())
        {
            if (ULobbyWidget* LobbyUI = Cast<ULobbyWidget>(GetCurrentWidget()))
            {
                LobbyUI->UpdatePlayerList();
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("SERVER: Player [%s] changed Ready Status to: %s"),
            *PS->GetPlayerName(), bNewReadyStatus ? TEXT("READY") : TEXT("NOT READY"));

        // 서버에서 모든 플레이어가 준비되었는지 체크
        if (AMenuGameMode* GM = Cast<AMenuGameMode>(GetWorld()->GetAuthGameMode()))
        {
            if (GM->AreAllPlayersReady())
            {
                // 모두 준비되었다면 게임 시작 (레벨 이동)
                // GetWorld()->ServerTravel(TEXT("/Game/Maps/GameLevelName?listen"));
            }
        }
    }
}

void AMenuPlayerController::RequestStartGame()
{
    if (HasAuthority())
    {
        Server_StartGame(); // 서버라면 즉시 실행
    }
    else
    {
        Server_StartGame(); // 클라이언트라면 서버에 요청
    }
}

bool AMenuPlayerController::Server_StartGame_Validate() { return true; }

void AMenuPlayerController::Server_StartGame_Implementation()
{
    // 이동하기 전, 마우스를 끄고 입력 모드를 기본으로 돌려줍니다.
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());

    if (AMenuGameMode* GM = Cast<AMenuGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GM->StartGame();
    }
}