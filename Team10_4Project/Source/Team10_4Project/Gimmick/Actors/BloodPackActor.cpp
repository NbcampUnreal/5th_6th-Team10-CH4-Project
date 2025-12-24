// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/BloodPackActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Character/CivilianAttributeSet.h"
#include "GameplayAbilitiesModule.h"
#include "Character/Civilian.h"
#include "GameplayTagContainer.h"
#include "Components/WidgetComponent.h"
#include "Gimmick/UI/InteractionWidgetBase.h"

// Sets default values
ABloodPackActor::ABloodPackActor()
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
	InteractionWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}

// Called when the game starts or when spawned
void ABloodPackActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABloodPackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 상호작용 로직 구현 (GE 적용)
void ABloodPackActor::Interact_Implementation(AActor* _Instigator)
{
	// 이 함수는 플레이어 캐릭터가 서버에게 Server_RPC하여 서버에서 실행됩니다.
	if (!HasAuthority())
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Server: BloodPack Interact Logic Started."));

	if (!_Instigator || !GEClassBloodPack)
	{
		UE_LOG(LogTemp, Error, TEXT("Server: Instigator or GEClassBloodPack is NULL!"));
		return;
	}

	// _Instigator (상호작용 요청자의 ASC(AbilitySystemComponent) 가져오기
	ACivilian* CivilianCharacter = Cast<ACivilian>(_Instigator);
	if (!CivilianCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Server: Instigator is NOT ACivilian!"));
		return;
	}

	UAbilitySystemComponent* _InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);
	//UAbilitySystemComponent* _InstigatorASC = Cast<ACivilian>(_Instigator)->AbilitySystemComponent;
	if (_InstigatorASC)
	{
		FGameplayTag InfectedTag = FGameplayTag::RequestGameplayTag(FName("PlayerRole.Infected"));
		if(_InstigatorASC->HasMatchingGameplayTag(InfectedTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Instigator is Infected. Try Consume BloodPack."));
			const UCivilianAttributeSet* CivilianAS = _InstigatorASC->GetSet<UCivilianAttributeSet>();
			if (!CivilianAS)
			{
				// 이 로그가 뜬다면 ASC는 존재하지만, AS는 초기화되지 않은 것.
				UE_LOG(LogTemp, Error, TEXT("Server: FAILED! Target ASC found, but Civilian Attribute Set (CivilianAS) is missing on ASC."));
				return;
			}

			// Gameplay Effect Context 생성
			FGameplayEffectContextHandle EffectContext = _InstigatorASC->MakeEffectContext();
			EffectContext.AddInstigator(_Instigator, this); // 누가 사용했는지, 원인이 무엇인지 기록

			// Gameplay Effect Spec 생성
			FGameplayEffectSpecHandle SpecHandle = _InstigatorASC->MakeOutgoingSpec(
				GEClassBloodPack,
				1.0f,
				EffectContext
			);

			if (SpecHandle.IsValid())
			{
				// GE 적용
				FActiveGameplayEffectHandle ActiveGEHandle = _InstigatorASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				float CurrentSanity = CivilianAS->GetSanity();
				UE_LOG(LogTemp, Warning, TEXT("Server: BloodPack Consumed. [Current Sanity: %.2f]"), CurrentSanity);

				UE_LOG(LogTemp, Warning, TEXT("Server: GE Applied Successfully. Destroying Actor."));
				Destroy();
			}
			else
			{
				// SpecHandle 생성 실패 (GEClassBloodPack이 널인 경우)
				UE_LOG(LogTemp, Error, TEXT("Server: GE Spec Handle is invalid (GEClassBloodPack is likely NULL)."));
			}
		}
	}
}

// 마우스 올렸을 때 표시할 텍스트 (예: Press E to interact)
FText ABloodPackActor::GetInteractText_Implementation(AActor* _Instigator) const
{
	// 로컬 플레이어의 ASC 확인
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);

	if (ASC)
	{
		FGameplayTag InfectedTag = FGameplayTag::RequestGameplayTag(FName("PlayerRole.Infected"));

		if (ASC->HasMatchingGameplayTag(InfectedTag))
		{
			return FText::FromString(TEXT("혈액팩 사용 (E)"));
		}
	}

	// 감염자가 아니거나 ASC를 찾을 수 없는 경우 - 아무것도 반환하지 않음. 없음.
	return FText::FromString(TEXT(""));
	//return FText::FromString(TEXT("조사하기 (E)"));
	//return NSLOCTEXT("Gimmick", "BloodPack_Interact", "혈액팩 섭취하기");
}

void ABloodPackActor::SetInteractionUI_Implementation(bool bVisible, AActor* _Instigator)
{
	if (!InteractionWidget || !_Instigator) return;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);
	FGameplayTag InfectedTag = FGameplayTag::RequestGameplayTag(FName("PlayerRole.Infected"));
	if (IsValid(ASC) == true) {
		if (ASC->HasMatchingGameplayTag(InfectedTag))	// 감염자일 때만 원래 의도대로 UI를 켜거나 끔
		{
			InteractionWidget->SetVisibility(bVisible);
			if (bVisible)
			{
				// 어차피 여기까지 왔다면 감염자이므로 "혈액팩 섭취" 문구만 띄우면 됩니다.
				FText Label = FText::FromString(TEXT("혈액팩 섭취 (E)"));

				if (UInteractionWidgetBase* WBP = Cast<UInteractionWidgetBase>(InteractionWidget->GetUserWidgetObject()))
				{
					WBP->UpdateText(Label);
				}
			}
		}
		else {	// 감염자가 아니라면 bVisible이 true로 들어와도 강제로 false 처리
			InteractionWidget->SetVisibility(false);
			return;
		}
	}
}