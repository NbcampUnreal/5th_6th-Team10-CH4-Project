// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/AreaDoor.h"
#include "GameMode/Team10GameMode.h"
#include "GameMode/Manager/GameFlowManager.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/GameTypes/GameTypes.h"
#include "GameState/Team10GameState.h"

// Sets default values
AAreaDoor::AAreaDoor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = DoorMesh;
}

// Called when the game starts or when spawned
void AAreaDoor::BeginPlay()
{
	Super::BeginPlay();
	
    // GameState를 찾아 델리게이트 바인딩
    if (ATeam10GameState* GS = GetWorld()->GetGameState<ATeam10GameState>())
    {
        GS->OnAreaChanged.AddDynamic(this, &AAreaDoor::HandleAreaChanged);
    }

    // 서버에서
    if (HasAuthority())
    {
        // GameState를 가져와서 델리게이트 구독
        if (ATeam10GameState* GS = GetWorld()->GetGameState<ATeam10GameState>())
        {
            GS->OnAreaChanged.AddDynamic(this, &AAreaDoor::HandleAreaChanged);
        }
    }
}

void AAreaDoor::HandleAreaChanged(FGameplayTag NewAreaTag)
{
    //// 게임의 새 구역이 이 문이 목표로 하는 구역과 일치하면 문을 염.
    //if (NewArea == TargetArea && !bIsDoorOpen)
    //{
    //    bIsDoorOpen = true;
    //    OnRep_IsDoorOpen(); // 서버에서도 실행
    //}
    // 방송된 태그가 내 목표 태그와 일치하면 문 오픈
    if (NewAreaTag.MatchesTagExact(TargetAreaTag))
    {
        OpenDoor(); // 기존에 만든 오픈 로직 호출
    }
}

void AAreaDoor::OnRep_IsDoorOpen()
{
    if (bIsDoorOpen)
    {
        OpenDoor(); // 문이 열리는 효과 실행
    }
}

void AAreaDoor::OpenDoor_Implementation()
{
    DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DoorMesh->SetVisibility(false);
    //UE_LOG(LogTemp, Warning, TEXT("Door for %s is now OPEN!"), *UEnum::GetValueAsString(TargetArea));
}

void AAreaDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAreaDoor, bIsDoorOpen);
}