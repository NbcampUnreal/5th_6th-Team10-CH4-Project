#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h" // 버튼 컴포넌트 필수
#include "HostGuestWidget.generated.h"

UCLASS()
class TEAM10_4PROJECT_API UHostGuestWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 위젯 초기화 시 버튼 이벤트를 소스에서 직접 바인딩합니다.
	virtual void NativeConstruct() override;

	// WBP에 배치한 버튼과 이름을 정확히 맞춰야 합니다. (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Host;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Guest;

	// 소스 전용 클릭 핸들러
	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnGuestClicked();
};