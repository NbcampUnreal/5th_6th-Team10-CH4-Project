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

public:
    // [추가] 외부(PlayerState 등)에서 호출할 수 있도록 public에 선언
    void UpdatePlayerList();

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
    class UButton* StartButton;

    UFUNCTION()
    void OnStartButtonClicked();
};