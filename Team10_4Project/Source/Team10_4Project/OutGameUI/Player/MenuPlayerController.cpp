#include "MenuPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (!IsLocalController())
        return;

    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());

    ShowMainMenu();
}

void AMenuPlayerController::ShowMainMenu()
{
    if (CurrentWidget)
        CurrentWidget->RemoveFromParent();

    CurrentWidget = CreateWidget(this, LoadClass<UUserWidget>(
        nullptr,
        TEXT("/Game/Team10/OutGameUI/UI/WBP_MainMenu.WBP_MainMenu_C")
    ));

    if (CurrentWidget)
        CurrentWidget->AddToViewport();
}

void AMenuPlayerController::ShowServerBrowser()
{
    if (CurrentWidget)
        CurrentWidget->RemoveFromParent();

    CurrentWidget = CreateWidget(this, LoadClass<UUserWidget>(
        nullptr,
        TEXT("/Game/Team10/OutGameUI/UI/WBP_ServerBrowser.WBP_ServerBrowser_C")
    ));

    if (CurrentWidget)
        CurrentWidget->AddToViewport();
}
