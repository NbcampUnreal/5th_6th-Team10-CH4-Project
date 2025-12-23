// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/AmmoBoxActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Character/CivilianAttributeSet.h"
#include "GameplayAbilitiesModule.h"
#include "Character/Civilian.h"

// Sets default values
AAmmoBoxActor::AAmmoBoxActor()
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
}

// Called when the game starts or when spawned
void AAmmoBoxActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAmmoBoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 상호작용 로직 구현 (GE 적용)
void AAmmoBoxActor::Interact_Implementation(AActor* _Instigator)
{
	// 이 함수는 플레이어 캐릭터가 서버에게 Server_RPC하여 서버에서 실행됩니다.
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Server: AmmoBox Interact Logic Started."));

	if (!_Instigator || !GEClassAmmoBox)
	{
		UE_LOG(LogTemp, Error, TEXT("Server: Instigator or GEClassAmmoBox is NULL!"));
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
			GEClassAmmoBox,
			1.0f,
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			// GE 적용
			FActiveGameplayEffectHandle ActiveGEHandle = _InstigatorASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			float CurrentAmmo = CivilianAS->GetAmmo();
			UE_LOG(LogTemp, Warning, TEXT("Server: AmmoBox Consumed. [Current Ammo: %.0f]"), CurrentAmmo);

			UE_LOG(LogTemp, Warning, TEXT("Server: GE Applied Successfully. Destroying Actor."));
			Destroy();
		}
		else
		{
			// SpecHandle 생성 실패 (GEClassAmmoBox이 널인 경우)
			UE_LOG(LogTemp, Error, TEXT("Server: GE Spec Handle is invalid (GEClassAmmoBox is likely NULL)."));
		}
	}
}

FText AAmmoBoxActor::GetInteractText_Implementation() const
{
	return NSLOCTEXT("Gimmick", "AmmoBox_Interact", "혈액팩 섭취하기");
}