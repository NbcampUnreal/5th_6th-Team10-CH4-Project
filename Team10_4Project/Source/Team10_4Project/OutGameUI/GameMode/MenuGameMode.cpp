#include "MenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (!MainMenuWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass is not set"));
        return;
    }

    UUserWidget* MainMenu = CreateWidget<UUserWidget>(
        GetWorld(),
        MainMenuWidgetClass
    );

    if (MainMenu)
    {
        MainMenu->AddToViewport();
    }
}
