#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION()
    void ShowMainMenu();

    UFUNCTION()
    void ShowServerBrowser();

private:
    UPROPERTY()
    UUserWidget* CurrentWidget;
};
