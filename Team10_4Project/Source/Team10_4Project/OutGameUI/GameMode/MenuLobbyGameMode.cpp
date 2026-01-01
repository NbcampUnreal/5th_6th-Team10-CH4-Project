#include "OutGameUI/GameMode/MenuLobbyGameMode.h"
#include "OutGameUI/Player/MenuPlayerController.h"
#include "OutGameUI/Player/MenuPlayerState.h"
#include "GameFramework/GameStateBase.h"

AMenuLobbyGameMode::AMenuLobbyGameMode()
{
    PlayerControllerClass = AMenuPlayerController::StaticClass();
    PlayerStateClass = AMenuPlayerState::StaticClass();
}

void AMenuLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // [이사 완료] L_Lobby 레벨에 접속한 사람에게만 로비 UI를 생성함
    if (AMenuPlayerController* PC = Cast<AMenuPlayerController>(NewPlayer))
    {
        PC->ShowLobby();
    }
    UE_LOG(LogTemp, Log, TEXT("Post Login: Player entered L_Lobby Level"));
}

bool AMenuLobbyGameMode::AreAllPlayersReady() const
{
    if (!GameState) return false;
    for (APlayerState* PS : GameState->PlayerArray)
    {
        if (AMenuPlayerState* MPS = Cast<AMenuPlayerState>(PS))
        {
            if (!MPS->IsReady()) return false;
        }
    }
    return true;
}

void AMenuLobbyGameMode::StartGame()
{
    bUseSeamlessTravel = false;
    GetWorld()->ServerTravel(TEXT("/Game/team10/Level/InGame"));
}