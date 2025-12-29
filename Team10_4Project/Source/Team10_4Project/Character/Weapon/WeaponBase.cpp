// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 3인칭 메쉬 설정
	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	WeaponMesh3P->SetupAttachment(RootComponent);
	WeaponMesh3P->SetOwnerNoSee(true); // 주인(나)에게는 안 보임
	WeaponMesh3P->bCastHiddenShadow = true; // 그림자는 보여야 함

	// 1인칭 메쉬 설정
	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	WeaponMesh1P->SetupAttachment(RootComponent);
	WeaponMesh1P->SetOnlyOwnerSee(true); // 주인(나)에게만 보임
	WeaponMesh1P->SetCastShadow(false); // 1인칭 팔 그림자는 보통 끕니다
}


