#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerSlotWidget.generated.h"

UCLASS()
class TEAM10_4PROJECT_API UServerSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    // 서버 정보 세팅용 함수
    void SetServerInfo(
        const FString& InServerName,
        int32 InCurrentPlayers,
        int32 InMaxPlayers,
        int32 InPing
    );

protected:
    UFUNCTION()
    void OnClick_Join();

protected:
    // --- BindWidget 영역 ---

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ServerNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerCountText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PingText;

    UPROPERTY(meta = (BindWidget))
    class UButton* JoinButton;
};
