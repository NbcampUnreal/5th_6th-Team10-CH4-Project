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

    // 위젯 표시 함수들
    void ShowMainMenu();
    void ShowServerBrowser();
    void ShowLobby();

    // 준비 상태 및 게임 시작 관련
    void RequestToggleReady();
    UFUNCTION(Server, Reliable)
    void Server_ToggleReady();

    void RequestStartGame();
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_StartGame();

    // [중요] 실제 레벨 이동을 담당하는 함수
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void JoinLobbyLevel();

    UUserWidget* GetCurrentWidget() const { return CurrentWidget; }

    void ShowHostGuestMenu();

protected:
    UPROPERTY()
    UUserWidget* CurrentWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UServerBrowserWidget> ServerBrowserWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<ULobbyWidget> LobbyWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UHostGuestWidget> HostGuestWidgetClass;

private:
    void ClearCurrentWidget();
};