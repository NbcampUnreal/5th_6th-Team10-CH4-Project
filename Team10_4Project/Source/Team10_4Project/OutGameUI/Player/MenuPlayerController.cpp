#include "OutGameUI/Player/MenuPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "OutGameUI/UI/MainMenuWidget.h"
#include "OutGameUI/UI/ServerBrowserWidget.h"
#include "OutGameUI/UI/LobbyWidget.h"

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());

    ShowMainMenu();
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

    if (!LobbyWidgetClass) return;

    CurrentWidget = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();
    }
}
