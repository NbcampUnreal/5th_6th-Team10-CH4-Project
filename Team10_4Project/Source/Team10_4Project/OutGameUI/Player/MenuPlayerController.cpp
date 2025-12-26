#include "MenuPlayerController.h"

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

void AMenuPlayerController::ChangeWidget(UUserWidget* NewWidget)
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    CurrentWidget = NewWidget;

    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();
    }
}

void AMenuPlayerController::ShowMainMenu()
{
    if (!MainMenuWidgetClass) return;

    UMainMenuWidget* Widget =
        CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);

    ChangeWidget(Widget);
}

void AMenuPlayerController::ShowServerBrowser()
{
    if (!ServerBrowserWidgetClass) return;

    UServerBrowserWidget* Widget =
        CreateWidget<UServerBrowserWidget>(this, ServerBrowserWidgetClass);

    ChangeWidget(Widget);
}

void AMenuPlayerController::ShowLobby()
{
    if (!LobbyWidgetClass) return;

    ULobbyWidget* Widget =
        CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);

    ChangeWidget(Widget);
}
