// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Actors/BloodPackActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Gimmick/GAS/AS_Deceit.h"

// Sets default values
ABloodPackActor::ABloodPackActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// SphereComp 생성 후 루트로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(50.0f);

	// StaticMeshComponent 생성 후 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);

	// 액터가 Network 상에서 복제되도록 설정
	SetReplicates(true);
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
	UE_LOG(LogTemp, Warning, TEXT("Server: BloodPack Interact Logic Started."));

	if (!_Instigator || !GEClassBloodPack)
	{
		return;
	}

	// _Instigator (상호작용 요청자의 ASC(AbilitySystemComponent) 가져오기
	UAbilitySystemComponent* _InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(_Instigator);
	if (_InstigatorASC)
	{
		const UAS_Deceit* DeceitAS = _InstigatorASC->GetSet<UAS_Deceit>();
		if (!DeceitAS)
		{
			// 이 로그가 뜬다면 ASC는 존재하지만, AS는 초기화되지 않은 것.
			UE_LOG(LogTemp, Error, TEXT("Server: FAILED! Target ASC found, but Deceit Attribute Set (AS_DeCeit) is missing on ASC."));
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Server: PASSED! DeceitAttribute Set Found! TransformGauge: %f"), DeceitAS->GetTransformGauge());

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
			if (ActiveGEHandle.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Server: GE Applied Successfully. Destroying Actor."));

				// GE 적용 성공, 액터 파괴 (서버에서 파괴하면 클라이언트에 자동으로 파괴가 복제됨.
				Destroy();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Server: Failed to apply GE. Check GE_BloodPackConsume setup."));
			}
		}
		else
		{	
			// SpecHandle 생성 실패 (GEClassBloodPack이 널인 경우)
			UE_LOG(LogTemp, Error, TEXT("Server: GE Spec Handle is invalid (GEClassBloodPack is likely NULL)."));
		}
	}
}

FText ABloodPackActor::GetInteractText_Implementation() const
{
	return NSLOCTEXT("Gimmick", "BloodPack_Interact", "혈액팩 섭취하기");
}