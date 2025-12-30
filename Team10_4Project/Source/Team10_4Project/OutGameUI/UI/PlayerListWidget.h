#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListWidget.generated.h"

class UScrollBox;
class UTextBlock;

UCLASS()
class TEAM10_4PROJECT_API UPlayerListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;

    void AddPlayer(const FString& PlayerName, bool bIsReady);

    // 전체 초기화
    void ClearPlayers();

protected:
    // 플레이어 목록 스크롤
    UPROPERTY(meta = (BindWidget))
    UScrollBox* PlayerListScrollBox;

    // 플레이어 한 줄 위젯 (WBP_PlayerEntry)
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PlayerEntryWidgetClass;
};