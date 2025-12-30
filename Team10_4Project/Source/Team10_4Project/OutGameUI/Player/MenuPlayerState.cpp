#include "MenuPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OutGameUI/Player/MenuPlayerController.h"
#include "OutGameUI/UI/LobbyWidget.h"
#include "Engine/World.h"

AMenuPlayerState::AMenuPlayerState()
{
    bReplicates = true;
}

void AMenuPlayerState::SetReady(bool bNewReady)
{
    if (HasAuthority())
    {
        bIsReady = bNewReady;
        // 서버에서도 UI 갱신을 위해 호출
        OnRep_Ready();
    }
}

void AMenuPlayerState::OnRep_Ready()
{
    // 1. 일단 복제가 오는지 확인 (모든 PS에서 다 찍힘)
    // UE_LOG(LogTemp, Warning, TEXT("OnRep_Ready Debug: Variable Replicated for %s"), *GetPlayerName());

    // 2. 이 컴퓨터의 주인(로컬 플레이어)을 찾음
    if (UWorld* World = GetWorld())
    {
        AMenuPlayerController* PC = Cast<AMenuPlayerController>(World->GetFirstPlayerController());

        // 3. 로컬 컨트롤러가 있고, 그 컨트롤러가 실제로 조종 중인 경우
        if (PC && PC->IsLocalController())
        {
            // 4. GetCurrentWidget()이 nullptr인지 확인하기 위해 로그 세분화
            UUserWidget* CurrentUI = PC->GetCurrentWidget();
            if (!CurrentUI)
            {
                // UE_LOG(LogTemp, Error, TEXT("OnRep_Ready: CurrentWidget is NULL!"));
                return;
            }

            ULobbyWidget* LobbyUI = Cast<ULobbyWidget>(CurrentUI);
            if (LobbyUI)
            {
                UE_LOG(LogTemp, Warning, TEXT("UI Update Triggered for local player!"));
                LobbyUI->UpdatePlayerList();
            }
            else
            {
                // UE_LOG(LogTemp, Error, TEXT("OnRep_Ready: CurrentWidget is NOT ULobbyWidget! (Actual: %s)"), *CurrentUI->GetName());
            }
        }
    }
}

void AMenuPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMenuPlayerState, bIsReady);
}