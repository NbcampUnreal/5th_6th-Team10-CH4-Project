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

    UFUNCTION(Server, Reliable)
    void Server_ToggleReady();

    void RequestToggleReady();

    void RequestStartGame();

    // 실제 서버에서 실행될 RPC
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_StartGame();

    UUserWidget* GetCurrentWidget() const { return CurrentWidget; }

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
