#include "MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OutGameUI/Player/MenuPlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("MainMenuWidget NativeConstruct"));

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Play);
    }

    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Settings);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Quit);
    }
}

void UMainMenuWidget::OnClick_Play()
{
    /*if (auto* PC = Cast<AMenuPlayerController>(GetOwningPlayer()))
    {
        PC->ShowServerBrowser();
    }*/ // 기존 showserverbroswer 로직

    if (AMenuPlayerController* PC = Cast<AMenuPlayerController>(GetOwningPlayer()))
    {
        // 서버 브라우저 대신 호스트/게스트 선택창을 띄웁니다.
        PC->ShowHostGuestMenu();
    }
}



void UMainMenuWidget::OnClick_Settings()
{
    UE_LOG(LogTemp, Log, TEXT("Settings 버튼 클릭"));
}

void UMainMenuWidget::OnClick_Quit()
{
    UKismetSystemLibrary::QuitGame(
        this,
        GetOwningPlayer(),
        EQuitPreference::Quit,
        false
    );
}
