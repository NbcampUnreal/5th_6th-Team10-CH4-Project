#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

UCLASS()
class TEAM10_4PROJECT_API ULobbyWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* LeaveButton;

    UFUNCTION()
    void OnClick_Leave();
};
