#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

class UMainMenuWidget;
class UServerBrowserWidget;

UCLASS()
class TEAM10_4PROJECT_API AMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    // 메인 메뉴 표시
    UFUNCTION()
    void ShowMainMenu();

    // 서버 브라우저 표시
    UFUNCTION()
    void ShowServerBrowser();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UServerBrowserWidget> ServerBrowserWidgetClass;

private:
    UPROPERTY()
    UUserWidget* CurrentWidget;
};
