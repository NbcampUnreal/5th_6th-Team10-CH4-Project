// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AttackBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_AttackBase::UGA_AttackBase()
{
	// 기본 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

FHitResult UGA_AttackBase::PerformLineTrace()
{
	FHitResult HitResult;
	
	// 시전자(Avatar)와 컨트롤러 가져오기
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (!Character) return HitResult;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC) return HitResult;

	// 카메라 위치와 방향 가져오기 (1인칭 시점 기준)
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	// 트레이스 시작점과 끝점 계산
	FVector Start = CamLoc;
	FVector End = Start + (CamRot.Vector() * AttackRange);

	// 트레이스 파라미터 설정 (자신은 무시)
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(AvatarActor);

	// 실제 레이캐스트 발사
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility, // 또는 ECC_GameTraceChannel1 (Weapon)
		Params
	);

	// (디버그용) 빨간 선 그리기 - 개발 중에만 켜세요
#if WITH_EDITOR
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 1.0f);
#endif

	return HitResult;
}

bool UGA_AttackBase::GetEnemiesInRadius(float Radius, TArray<FHitResult>& OutHits)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;

	FVector Start = AvatarActor->GetActorLocation();
	FVector End = Start + (AvatarActor->GetActorForwardVector() * Radius);

	// 구체 모양으로 휩쓸기 (Sweep)
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn, // 캐릭터만 감지
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(FName("MeleeAttack"), false, AvatarActor)
	);

	return bHit;
}

void UGA_AttackBase::ApplyDamageToTarget(AActor* TargetActor)
{
	if (!TargetActor || !DamageEffectClass) return;

	// 1. ASC 가져오기
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (SourceASC && TargetASC)
	{
		// 2. GameplayEffect Spec 생성 (Context 포함)
		FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this); // 소스 설정

		// 레벨은 1.0f로 설정 (필요시 변수화)
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);

		// 3. 데미지 수치 동적 할당 (SetByCaller)
		// GE_Damage 내부에 'Damage'라는 Tag로 SetByCaller가 설정되어 있어야 함
		FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTag, AttackDamage);

		// 4. 적용
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}
