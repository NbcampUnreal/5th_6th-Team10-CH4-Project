#include "MenuGameMode.h"
#include "OutGameUI/Player/MenuPlayerController.h"

AMenuGameMode::AMenuGameMode()
{
    // UI 전용 컨트롤러
    PlayerControllerClass = AMenuPlayerController::StaticClass();

    // Pawn / Character 생성 안 함
    DefaultPawnClass = nullptr;
}