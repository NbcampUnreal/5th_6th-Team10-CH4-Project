// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/FuseActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GamePlayTag/GamePlayTags.h"
#include "Components/WidgetComponent.h"
#include "GameState/Team10GameState.h"
#include "Gimmick/UI/InteractionWidgetBase.h"
#include "InGameUI/KSH/InventoryComponent.h"
#include "Character/CivilianPlayerState.h"

// Sets default values
AFuseActor::AFuseActor()
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

	// InteractionWidget 생성 후 설정
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
}

// Called when the game starts or when spawned
void AFuseActor::BeginPlay()
{
	Super::BeginPlay();

	ATeam10GameState* Team10GameState = GetWorld()->GetGameState<ATeam10GameState>();
	if (!Team10GameState)
	{
		return;
	}

	Team10GameState->OnGamePhaseChanged.AddDynamic(this, &AFuseActor::SetFuseHiddenAtDayPhase);
}

// Called every frame
void AFuseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFuseActor::SetFuseHiddenAtDayPhase(EGamePhase GamePhase)
{
	SetActorHiddenInGame(GamePhase == EGamePhase::DayPhase); // 낮 페이즈 때만 퓨즈 숨김
}

// 상호작용
void AFuseActor::Interact_Implementation(AActor* _Instigator)
{
	if (HasAuthority())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);
		if (ASC && HasFuseGEClass)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddInstigator(_Instigator, _Instigator);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HasFuseGEClass, 1.0f, EffectContext);
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
			
			// 인벤토리에 아이템 추가
			APawn* InstigatorPawn = Cast<APawn>(_Instigator);
			if (!InstigatorPawn) return;

			if (ACivilianPlayerState* PS = InstigatorPawn->GetPlayerState<ACivilianPlayerState>())
			{
				if (PS->InventoryComponent)
				{
					PS->InventoryComponent->AddItemByID(FName("fuse"));
				}
			}

			Destroy();
		}
	}
}

FText AFuseActor::GetInteractText_Implementation(AActor* _Instigator) const
{
	// 로컬 플레이어의 ASC 확인
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);

	if (ASC)
	{
		FGameplayTag HasFuseTag = FGameplayTag::RequestGameplayTag(FName("Gimmick.Status_HasFuse"));
		if (!(ASC->HasMatchingGameplayTag(HasFuseTag)))
		{
			return FText::FromString(TEXT("퓨즈 가져가기 (E)"));
		}
	}

	// 퓨즈를 이미 소지하고 있거나 ASC를 찾을 수 없는 경우 - 아무것도 반환하지 않음. 없음.
	return FText::FromString(TEXT(""));
}

void AFuseActor::SetInteractionUI_Implementation(bool bVisible, AActor* _Instigator)
{
	if (!InteractionWidget || !_Instigator) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);
	if (ASC)
	{
		FGameplayTag HasFuseTag = FGameplayTag::RequestGameplayTag(FName("Gimmick.Status_HasFuse"));
		if (!(ASC->HasMatchingGameplayTag(HasFuseTag)))
		{
			InteractionWidget->SetVisibility(bVisible);
			if (bVisible)
			{
				FText Label = FText::FromString(TEXT("퓨즈 가져가기 (E)"));

				if (UInteractionWidgetBase* WBP = Cast<UInteractionWidgetBase>(InteractionWidget->GetUserWidgetObject()))
				{
					WBP->UpdateText(Label);
				}
			}
		}
		else
		{
			InteractionWidget->SetVisibility(false);
			return;
		}
	}
}