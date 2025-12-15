#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

bool UMainMenuWidget::Initialize()
{
    bool bResult = Super::Initialize();

    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Play);

    if (SettingsButton)
        SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Settings);

    if (QuitButton)
        QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnClick_Quit);

    return bResult;
}

void UMainMenuWidget::OnClick_Play()
{
    UE_LOG(LogTemp, Warning, TEXT("Play Clicked"));
}

void UMainMenuWidget::OnClick_Settings()
{
    UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
}

void UMainMenuWidget::OnClick_Quit()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
