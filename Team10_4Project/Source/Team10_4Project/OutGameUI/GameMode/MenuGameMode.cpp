#include "OutGameUI/GameMode/MenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "OutGameUI/Player/MenuPlayerState.h"

void AMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (!MainMenuWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainMenuWidgetClass is not set"));
        return;
    }

    UUserWidget* MainMenu = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
    if (MainMenu)
    {
        MainMenu->AddToViewport();
    }
}

bool AMenuGameMode::AreAllPlayersReady() const
{
    const int32 MinPlayers = 1;

    if (GameState == nullptr) return false;

    const TArray<APlayerState*>& Players = GameState->PlayerArray;

    if (Players.Num() < MinPlayers) return false;

    for (APlayerState* PS : Players)
    {
        if (!PS) continue;

        AMenuPlayerState* MenuPS = Cast<AMenuPlayerState>(PS);

        if (MenuPS)
        {
            if (!MenuPS->IsReady())
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("All players are READY!"));
    return true;
}

void AMenuGameMode::StartGame()
{
    // 모든 플레이어가 준비되었는지 확인하는 로직을 여기에 넣을 수 있습니다.
    // if (!AreAllPlayersReady()) return; 

    UWorld* World = GetWorld();
    if (World)
    {
        // bUseSeamlessTravel을 true로 설정하면 연결이 끊기지 않고 부드럽게 이동합니다.
        bUseSeamlessTravel = true;

        // 이동할 맵 경로 (예: /Game/Maps/MainGameMap)
        // ?listen 옵션은 이동한 맵에서도 서버 역할을 유지하겠다는 뜻입니다.
        FString MapPath = TEXT("/Game/team10/Level/InGame");
        World->ServerTravel(MapPath);
    }
}