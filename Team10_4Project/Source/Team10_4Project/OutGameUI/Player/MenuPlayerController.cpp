#include "OutGameUI/Player/MenuPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OutGameUI/UI/MainMenuWidget.h"
#include "OutGameUI/UI/ServerBrowserWidget.h"
#include "OutGameUI/UI/LobbyWidget.h"
#include "MenuPlayerState.h"
#include "OutGameUI/GameMode/MenuLobbyGameMode.h"
#include "OutGameUI/UI/HostGuestWidget.h"

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
    if (MainMenuWidgetClass)
    {
        CurrentWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
        if (CurrentWidget) CurrentWidget->AddToViewport();
    }
}

void AMenuPlayerController::ShowServerBrowser()
{
    ClearCurrentWidget();
    if (ServerBrowserWidgetClass)
    {
        CurrentWidget = CreateWidget<UServerBrowserWidget>(this, ServerBrowserWidgetClass);
        if (CurrentWidget) CurrentWidget->AddToViewport();
    }
}

void AMenuPlayerController::ShowLobby()
{
    ClearCurrentWidget();
    if (!LobbyWidgetClass) return;

    ULobbyWidget* LobbyWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
    if (LobbyWidget)
    {
        CurrentWidget = LobbyWidget;
        CurrentWidget->AddToViewport();

        // 마우스 커서 활성화
        bShowMouseCursor = true;

        // 위젯의 포커스 가능 여부를 코드에서도 명시적으로 확인/설정 가능
        LobbyWidget->SetIsFocusable(true);

        // 입력 모드 설정
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(LobbyWidget->GetCachedWidget()); // TakeWidget() 대신 GetCachedWidget() 시도
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        SetInputMode(InputMode);
    }
}

void AMenuPlayerController::JoinLobbyLevel()
{
    if (HasAuthority())
    {
        // 1. 경로가 실제 Content 폴더 기준 경로와 일치하는지 확인하세요
        FString LobbyPath = TEXT("/Game/team10/OutGameUI/L_Lobby");

        UE_LOG(LogTemp, Warning, TEXT("Attempting ServerTravel to: %s"), *LobbyPath);

        // 2. 월드 포인터 확인 후 실행
        if (UWorld* World = GetWorld())
        {
            World->ServerTravel(LobbyPath);
        }
    }
}

void AMenuPlayerController::RequestToggleReady() { Server_ToggleReady(); }

void AMenuPlayerController::Server_ToggleReady_Implementation()
{
    if (AMenuPlayerState* PS = GetPlayerState<AMenuPlayerState>())
    {
        PS->SetReady(!PS->IsReady());
        if (AMenuLobbyGameMode* LobbyGM = Cast<AMenuLobbyGameMode>(GetWorld()->GetAuthGameMode()))
        {
            // 전원 준비 시 로직은 필요에 따라 추가
        }
    }
}

void AMenuPlayerController::RequestStartGame() { Server_StartGame(); }
bool AMenuPlayerController::Server_StartGame_Validate() { return true; }
void AMenuPlayerController::Server_StartGame_Implementation()
{
    if (AMenuLobbyGameMode* LobbyGM = Cast<AMenuLobbyGameMode>(GetWorld()->GetAuthGameMode()))
    {
        LobbyGM->StartGame();
    }
}

void AMenuPlayerController::ShowHostGuestMenu()
{
    ClearCurrentWidget();

    if (HostGuestWidgetClass)
    {
        UHostGuestWidget* HostGuestWidget = CreateWidget<UHostGuestWidget>(this, HostGuestWidgetClass);
        if (HostGuestWidget)
        {
            CurrentWidget = HostGuestWidget;
            CurrentWidget->AddToViewport();

            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(CurrentWidget->TakeWidget());
            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }
}