#include "OutGameUI/UI/HostGuestWidget.h"
#include "InGameUI/JKH/EOSSubsystem.h"
#include "OutGameUI/Player/MenuPlayerController.h"

void UHostGuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 그래프를 쓰지 않고 소스에서 버튼 클릭 이벤트를 직접 연결합니다.
	if (Button_Host)
	{
		Button_Host->OnClicked.AddDynamic(this, &UHostGuestWidget::OnHostClicked);
	}

	if (Button_Guest)
	{
		Button_Guest->OnClicked.AddDynamic(this, &UHostGuestWidget::OnGuestClicked);
	}
}

void UHostGuestWidget::OnHostClicked()
{
	if (UEOSSubsystem* EOS = GetGameInstance()->GetSubsystem<UEOSSubsystem>())
	{
		// 다른 담당자가 만든 EOS 세션 생성 호출
		EOS->CreateLobbySession(4, false);
	}
}

void UHostGuestWidget::OnGuestClicked()
{
	// 사용자님의 설계대로 컨트롤러를 통해 위젯 전환
	if (AMenuPlayerController* PC = Cast<AMenuPlayerController>(GetOwningPlayer()))
	{
		PC->ShowServerBrowser();
	}
}