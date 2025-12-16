#include "ServerBrowserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"

void UServerBrowserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RefreshButton)
        RefreshButton->OnClicked.AddDynamic(this, &UServerBrowserWidget::OnClick_Refresh);

    if (BackButton)
        BackButton->OnClicked.AddDynamic(this, &UServerBrowserWidget::OnClick_Back);
}

void UServerBrowserWidget::OnClick_Refresh()
{
    UE_LOG(LogTemp, Warning, TEXT("Refresh Server List"));
}

void UServerBrowserWidget::OnClick_Back()
{
    RemoveFromParent();
}
