#include "MenuPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "OutGameUI/UI/MainMenuWidget.h"
#include "OutGameUI/UI/ServerBrowserWidget.h"

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass)
    {
        UUserWidget* Widget =
            CreateWidget(this, MainMenuWidgetClass);

        if (Widget)
        {
            Widget->AddToViewport();
            SetInputMode(FInputModeUIOnly());
            bShowMouseCursor = true;
        }
    }
}


void AMenuPlayerController::ShowMainMenu()
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    if (MainMenuWidgetClass)
    {
        CurrentWidget = CreateWidget(this, MainMenuWidgetClass);
        if (CurrentWidget)
        {
            CurrentWidget->AddToViewport();
        }
    }
}

void AMenuPlayerController::ShowServerBrowser()
{
    if (!ServerBrowserWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ServerBrowserWidgetClass is NULL"));
        return;
    }

    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    CurrentWidget = CreateWidget<UUserWidget>(
        this,
        ServerBrowserWidgetClass
    );

    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();

        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;

        UE_LOG(LogTemp, Log, TEXT("ServerBrowserWidget shown"));
    }
}

void AMenuPlayerController::ShowLobby()
{
    if (!LobbyWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyWidgetClass is NULL"));
        return;
    }

    UUserWidget* LobbyWidget =
        CreateWidget<UUserWidget>(this, LobbyWidgetClass);

    if (LobbyWidget)
    {
        LobbyWidget->AddToViewport();
    }
}