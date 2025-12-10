#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"   // ⭐ CommonUI 기반 Activatable Widget
#include "MainMenuWidget.generated.h"

/**
 * 메인 메뉴 UI의 최상위 화면.
 * - CommonUI 기반으로 화면 전환, 입력 포커스, 애니메이션 등을 자동 처리.
 * - 서버 브라우저 / 설정 / 게임 시작 등 OutGame 시스템의 중심 허브.
 */
UCLASS()
class TEAM10_4PROJECT_API UMainMenuWidget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

protected:
    UFUNCTION()
    void OnClick_Play();

    UFUNCTION()
    void OnClick_Settings();

    UFUNCTION()
    void OnClick_Quit();

protected:

    UPROPERTY(meta = (BindWidget))
    class UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* SettingsButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* QuitButton;
};