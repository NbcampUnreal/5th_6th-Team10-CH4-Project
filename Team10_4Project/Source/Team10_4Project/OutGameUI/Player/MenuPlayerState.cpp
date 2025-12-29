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
    // 내 상태가 바뀌었으니 내 화면의 로비 UI를 새로고침하라고 명령
    if (AMenuPlayerController* PC = Cast<AMenuPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        // PC->GetCurrentWidget()을 통해 현재 열려있는 로비 위젯을 가져와서 리스트 갱신
        // (LobbyWidget에 UpdatePlayerList 함수가 있다고 가정)
    }
}

void AMenuPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMenuPlayerState, bIsReady);
}