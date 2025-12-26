#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

class UMainMenuWidget;
class UServerBrowserWidget;
class ULobbyWidget;

UCLASS()
class TEAM10_4PROJECT_API AMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    void ShowMainMenu();
    void ShowServerBrowser();
    void ShowLobby();

protected:
    UPROPERTY()
    UUserWidget* CurrentWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UServerBrowserWidget> ServerBrowserWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<ULobbyWidget> LobbyWidgetClass;

private:
    void ClearCurrentWidget();
};
