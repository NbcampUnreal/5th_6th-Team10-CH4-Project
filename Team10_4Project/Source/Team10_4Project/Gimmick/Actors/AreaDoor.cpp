// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/AreaDoor.h"
#include "GameMode/Team10GameMode.h"
#include "GameMode/Manager/GameFlowManager.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/GameTypes/GameTypes.h"

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
	
    // 서버에서만 구역 변경 이벤트를 감시함.
    if (HasAuthority())
    {
        if (ATeam10GameMode* GM = Cast<ATeam10GameMode>(GetWorld()->GetAuthGameMode()))
        {
            if (UGameFlowManager* FlowMgr = GM->FindComponentByClass<UGameFlowManager>())
            {
                // GameFlowManager의 델리게이트에 바인딩
                FlowMgr->OnCurrentAreaChangedDelegate.AddUObject(this, &AAreaDoor::HandleAreaChanged);
            }
        }
    }
}

void AAreaDoor::HandleAreaChanged(EGameArea NewArea)
{
    // 게임의 새 구역이 이 문이 목표로 하는 구역과 일치하면 문을 염.
    if (NewArea == TargetArea && !bIsDoorOpen)
    {
        bIsDoorOpen = true;
        OnRep_IsDoorOpen(); // 서버에서도 실행
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
    UE_LOG(LogTemp, Warning, TEXT("Door for %s is now OPEN!"), *UEnum::GetValueAsString(TargetArea));
}

void AAreaDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAreaDoor, bIsDoorOpen);
}