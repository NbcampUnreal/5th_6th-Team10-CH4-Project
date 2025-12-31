#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuLobbyGameMode.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AMenuLobbyGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMenuLobbyGameMode();

    // 로비 레벨 입장 시 로비 UI를 띄우는 핵심 로직
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // 로비 인원 준비 상태 체크
    bool AreAllPlayersReady() const;

    // 인게임 출발
    void StartGame();
};