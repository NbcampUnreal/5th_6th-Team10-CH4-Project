// MenuPlayerState.cpp
#include "MenuPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "OutGameUI/Player/MenuPlayerController.h"
#include "OutGameUI/UI/LobbyWidget.h"

AMenuPlayerState::AMenuPlayerState()
{
    bReplicates = true; // 서버와 클라이언트 간 복제 활성화
}

void AMenuPlayerState::SetReady(bool bNewReady)
{
    if (HasAuthority()) // 서버에서만 값을 바꿀 수 있음
    {
        bIsReady = bNewReady;
        OnRep_Ready();
    }
}

void AMenuPlayerState::OnRep_Ready()
{
    // 로컬 화면의 컨트롤러를 찾음
    AMenuPlayerController* PC = Cast<AMenuPlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC)
    {
        // 컨트롤러가 들고 있는 LobbyWidget을 찾아 리스트 갱신 호출
        ULobbyWidget* LobbyUI = Cast<ULobbyWidget>(PC->GetCurrentWidget());
        if (LobbyUI)
        {
            LobbyUI->UpdatePlayerList();
        }
    }
}

void AMenuPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMenuPlayerState, bIsReady);
}