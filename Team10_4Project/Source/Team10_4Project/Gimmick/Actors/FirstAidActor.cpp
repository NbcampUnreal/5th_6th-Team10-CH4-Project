// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/FirstAidActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Character/CivilianAttributeSet.h"
#include "GameplayAbilitiesModule.h"
#include "Character/Civilian.h"
#include "Components/WidgetComponent.h"
#include "Gimmick/UI/InteractionWidgetBase.h"

// Sets default values
AFirstAidActor::AFirstAidActor()
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
void AFirstAidActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFirstAidActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 상호작용 로직 구현 (GE 적용)
void AFirstAidActor::Interact_Implementation(AActor* _Instigator)
{
	// 이 함수는 플레이어 캐릭터가 서버에게 Server_RPC하여 서버에서 실행됩니다.
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Server: FirstAid Interact Logic Started."));

	if (!_Instigator || !GEClassFirstAid)
	{
		UE_LOG(LogTemp, Error, TEXT("Server: Instigator or GEClassFirstAid is NULL!"));
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
			GEClassFirstAid,
			1.0f,
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			// GE 적용
			FActiveGameplayEffectHandle ActiveGEHandle = _InstigatorASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			float CurrentHealth = CivilianAS->GetHealth();
			UE_LOG(LogTemp, Warning, TEXT("Server: FirstAid Consumed. [Current Health: %.2f]"), CurrentHealth);

			UE_LOG(LogTemp, Warning, TEXT("Server: GE Applied Successfully. Destroying Actor."));
			Destroy();
		}
		else
		{
			// SpecHandle 생성 실패 (GEClassFirstAid이 널인 경우)
			UE_LOG(LogTemp, Error, TEXT("Server: GE Spec Handle is invalid (GEClassFirstAid is likely NULL)."));
		}
	}
}

FText AFirstAidActor::GetInteractText_Implementation(AActor* _Instigator) const
{
	return NSLOCTEXT("Gimmick", "FirstAid_Interact", "구급상자 사용 (E)");
}

void AFirstAidActor::SetInteractionUI_Implementation(bool bVisible, AActor* _Instigator)
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