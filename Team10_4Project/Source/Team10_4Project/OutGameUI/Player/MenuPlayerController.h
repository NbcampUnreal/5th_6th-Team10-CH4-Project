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
    void ChangeWidget(UUserWidget* NewWidget);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UServerBrowserWidget> ServerBrowserWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<ULobbyWidget> LobbyWidgetClass;

private:
    UPROPERTY()
    UUserWidget* CurrentWidget;
};
