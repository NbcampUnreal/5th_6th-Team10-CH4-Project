#include "OutGameUI/GameMode/MenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "OutGameUI/Player/MenuPlayerController.h"

AMenuGameMode::AMenuGameMode()
{
    PlayerControllerClass = AMenuPlayerController::StaticClass();
}

void AMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 메인 메뉴 위젯 생성 (유지)
    if (MainMenuWidgetClass)
    {
        UUserWidget* MainMenu = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenu)
        {
            MainMenu->AddToViewport();
        }
    }
}
// [중요] PostLogin 함수는 여기서 삭제되었습니다. 
// 메인 메뉴 레벨에서는 자동으로 로비 UI를 띄우지 않기 위함입니다.