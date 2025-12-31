#include "MenuPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OutGameUI/Player/MenuPlayerController.h"
#include "OutGameUI/UI/LobbyWidget.h"

AMenuPlayerState::AMenuPlayerState()
{
    bReplicates = true;
    NetUpdateFrequency = 100.0f;
}

void AMenuPlayerState::SetReady(bool bNewReady)
{
    if (HasAuthority())
    {
        if (bIsReady == bNewReady) return;
        bIsReady = bNewReady;
        OnRep_Ready();
    }
}

void AMenuPlayerState::OnRep_Ready()
{
    if (UWorld* World = GetWorld())
    {
        if (AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(World->GetFirstPlayerController()))
        {
            // 이제 ShowLobby()가 먼저 실행되어 위젯이 존재하므로 성공함
            if (ULobbyWidget* LobbyUI = Cast<ULobbyWidget>(MenuPC->GetCurrentWidget()))
            {
                LobbyUI->UpdatePlayerList();
            }
        }
    }
}

void AMenuPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMenuPlayerState, bIsReady);
}