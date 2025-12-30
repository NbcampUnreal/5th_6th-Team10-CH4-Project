#include "LobbyWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/UI/LobbyWidget.h"
#include "OutGameUI/UI/PlayerListWidget.h"
#include "OutGameUI/Player/MenuPlayerController.h"
#include "OutGameUI/Player/MenuPlayerState.h"
#include "GameFramework/GameStateBase.h" // 추가
#include "GameFramework/PlayerState.h"   // 추가

void ULobbyWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 기존 버튼 바인딩 유지
    if (LeaveButton)
    {
        LeaveButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnClick_Leave);
    }

    if (ReadyButton)
    {
        ReadyButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnReadyButtonClicked);
    }

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartButtonClicked);
    }

    // [수정] 실제 플레이어 리스트 업데이트 호출
    UpdatePlayerList();

    UE_LOG(LogTemp, Log, TEXT("LobbyWidget Initialized and PlayerList Updated"));
}

// [신규] 실제 플레이어 데이터를 UI에 반영하는 함수
void ULobbyWidget::UpdatePlayerList()
{
    if (!PlayerListWidget) return;

    // UI 리스트 초기화
    PlayerListWidget->ClearPlayers();

    // GameState의 PlayerArray를 순회하며 접속자 정보 추가
    AGameStateBase* GS = GetWorld()->GetGameState();
    if (GS)
    {
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (!PS) continue;

            // MenuPlayerState로 캐스팅하여 준비 상태 확인
            AMenuPlayerState* MPS = Cast<AMenuPlayerState>(PS);
            bool bIsReady = MPS ? MPS->IsReady() : false;

            PlayerListWidget->AddPlayer(PS->GetPlayerName(), bIsReady);
        }
    }
}

void ULobbyWidget::OnClick_Leave()
{
    if (auto* PC = Cast<AMenuPlayerController>(GetOwningPlayer()))
    {
        PC->ShowMainMenu();
    }
}

void ULobbyWidget::OnReadyButtonClicked()
{
    AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(GetOwningPlayer());
    if (MenuPC && MenuPC->PlayerState)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] Ready Button Clicked!"), *MenuPC->PlayerState->GetPlayerName());
        MenuPC->RequestToggleReady();
    }
}

void ULobbyWidget::OnStartButtonClicked()
{
    AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(GetOwningPlayer());
    if (MenuPC)
    {
        MenuPC->RequestStartGame();
    }
}

void ULobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
    {
        APlayerController* PC = GetOwningPlayer();
        if (PC && PC->HasAuthority())
        {
            StartButton->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            StartButton->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}