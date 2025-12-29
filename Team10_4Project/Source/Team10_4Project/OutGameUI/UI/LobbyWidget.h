#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton;
class UPlayerListWidget;

UCLASS()
class TEAM10_4PROJECT_API ULobbyWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* LeaveButton;

    UFUNCTION()
    void OnClick_Leave();

    UPROPERTY(meta = (BindWidget))
    UPlayerListWidget* PlayerListWidget;

    UPROPERTY(meta = (BindWidget))
    class UButton* ReadyButton;

    UFUNCTION()
    void OnReadyButtonClicked();

    UPROPERTY(meta = (BindWidget))
    class UButton* StartButton; // WBP_Lobby에 StartButton 이름으로 버튼 추가 필요

    UFUNCTION()
    void OnStartButtonClicked();
};
