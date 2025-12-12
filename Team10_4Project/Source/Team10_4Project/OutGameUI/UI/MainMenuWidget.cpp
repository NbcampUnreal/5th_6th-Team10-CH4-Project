#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


void UMainMenuWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Play);

    if (SettingsButton)
        SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Settings);

    if (QuitButton)
        QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Quit);
}

void UMainMenuWidget::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();

    if (PlayButton)
        PlayButton->OnClicked.RemoveAll(this);

    if (SettingsButton)
        SettingsButton->OnClicked.RemoveAll(this);

    if (QuitButton)
        QuitButton->OnClicked.RemoveAll(this);
}

void UMainMenuWidget::OnClick_Play()
{
    // TODO: 서버 브라우저 화면으로 이동 (WidgetStackPush)
    UE_LOG(LogTemp, Warning, TEXT("Play Button"));
}

void UMainMenuWidget::OnClick_Settings()
{
    // TODO: 설정 화면으로 이동
    UE_LOG(LogTemp, Warning, TEXT("Settings Button"));
}

void UMainMenuWidget::OnClick_Quit()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
    UE_LOG(LogTemp, Warning, TEXT("Quit Button"));
}
