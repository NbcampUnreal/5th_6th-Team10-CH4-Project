#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerBrowserWidget.generated.h"

UCLASS()
class TEAM10_4PROJECT_API UServerBrowserWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    /** 새로고침 버튼 */
    UFUNCTION()
    void OnClick_Refresh();

    /** 뒤로가기 버튼 */
    UFUNCTION()
    void OnClick_Back();

protected:
    /** 서버 목록 스크롤 영역 */
    UPROPERTY(meta = (BindWidget))
    class UScrollBox* ServerListScrollBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* RefreshButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* BackButton;
};
