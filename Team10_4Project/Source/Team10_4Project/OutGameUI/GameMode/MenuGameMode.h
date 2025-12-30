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

    bool AreAllPlayersReady() const;

    void StartGame();

    virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
    // 메인 메뉴 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MainMenuWidgetClass;

    /** Ready 완료 시 이동할 레벨 */
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName GamePlayLevelName;
};

