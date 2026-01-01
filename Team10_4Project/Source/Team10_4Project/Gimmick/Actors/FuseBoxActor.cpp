// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/FuseBoxActor.h"
#include "GameMode/Team10GameMode.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GamePlayTag/GamePlayTags.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Gimmick/UI/InteractionWidgetBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "InGameUI/KSH/InventoryComponent.h"
#include "Character/CivilianPlayerState.h"



// Sets default values
AFuseBoxActor::AFuseBoxActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// SphereComp 생성 후 루트로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(50.0f);

	// StaticMeshComponent 생성 후 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);

	FuseBoxDoorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuseBoxDoorMeshComponent"));
	FuseBoxDoorMeshComp->SetupAttachment(RootComponent);
	FuseBoxDoorMeshComp->SetVisibility(false);

	// InteractionWidget 생성 후 설정
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

}

// Called when the game starts or when spawned
void AFuseBoxActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFuseBoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFuseBoxActor::Interact_Implementation(AActor* _Instigator)
{
	if (bIsActivated) return;	// 이미 활성화된 경우 무시

	if (HasAuthority())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);

		// 태그 확인 (GE가 복제되므로 서버/클라 모두 확인 가능)
		if (ASC && ASC->HasMatchingGameplayTag(GamePlayTags::Gimmick::Status_HasFuse))
		{
			bIsActivated = true;
			OnRep_IsActivated();

			// 태그를 들고 있게 만든 GE를 제거 (해당 태그를 '부여'하는 모든 GE 제거)
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(GamePlayTags::Gimmick::Status_HasFuse);
			ASC->RemoveActiveEffectsWithGrantedTags(TagContainer);

			// GameMode 알림
			if (ATeam10GameMode* GM = GetWorld()->GetAuthGameMode<ATeam10GameMode>())
			{
				GM->OnFuseBoxActivated();
			}

			// 인벤토리에서 아이템 제거
			APawn* InstigatorPawn = Cast<APawn>(_Instigator);
			if (!InstigatorPawn) return;

			if (ACivilianPlayerState* PS = InstigatorPawn->GetPlayerState<ACivilianPlayerState>())
			{
				if (PS->InventoryComponent)
				{
					PS->InventoryComponent->RemoveItemByID(FName("fuse"));
				}
			}
		}
	}

	//if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(_Instigator))
	//{
	//	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	//	
	//	//플레이어가 퓨즈를 소지하고 있는지 체크
	//	if (ASC && ASC->HasMatchingGameplayTag(GamePlayTags::Gimmick::Status_HasFuse))
	//	{
	//		if (HasAuthority())
	//		{
	//			bIsActivated = true;
	//			OnRep_IsActivated();	// 서버에서도 실행

	//			/// 플레이어의 퓨즈 소지 태그 제거
	//			ASC->RemoveLooseGameplayTag(GamePlayTags::Gimmick::Status_HasFuse);

	//			// GameMode에 알림
	//			if (ATeam10GameMode* GM = GetWorld()->GetAuthGameMode<ATeam10GameMode>())
	//			{
	//				GM->OnFuseBoxActivated();
	//			}
	//		}
	//	}
	//}
}

void AFuseBoxActor::OnRep_IsActivated()
{
	// 퓨즈 박스에 퓨즈를 꽂을 시 효과 코드
	if (bIsActivated)
	{
		if (FuseBoxDoorMeshComp)
		{
			FuseBoxDoorMeshComp->SetVisibility(true);
		}
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(false);
		}
	}
}

void AFuseBoxActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFuseBoxActor, bIsActivated);
}

FText AFuseBoxActor::GetInteractText_Implementation(AActor* _Instigator) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);
	if (ASC && ASC->HasMatchingGameplayTag(GamePlayTags::Gimmick::Status_HasFuse))
	{
		return NSLOCTEXT("Gimmick", "FuseBoxActor_Interact_HasFuse", "퓨즈 꽂기 (E)");
	}
	else return FText::FromString(TEXT(""));	// 퓨즈가 없으면 아무 텍스트도 표시하지 않음
}

void AFuseBoxActor::SetInteractionUI_Implementation(bool bVisible, AActor* _Instigator)
{
	if (!InteractionWidget) return;
	InteractionWidget->SetVisibility(bVisible);

	if (bVisible && _Instigator)
	{
		// 인터페이스를 통해 자식 클래스들이 정의한 텍스트를 가져옴
		FText NewText = IInteractable::Execute_GetInteractText(this, _Instigator);

		// 위젯을 캐스팅하여 블루프린트 이벤트 호출
		if (UInteractionWidgetBase* WBP = Cast<UInteractionWidgetBase>(InteractionWidget->GetUserWidgetObject()))
		{
			WBP->UpdateText(NewText);
		}
	}
}