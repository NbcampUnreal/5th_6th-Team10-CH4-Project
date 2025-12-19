#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AMenuGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    // 메인 메뉴 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MainMenuWidgetClass;
};
