// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

protected:
	// 3인칭용 메쉬 (타인이 보는 내 총)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh3P;

	// 1인칭용 메쉬 (내가 보는 내 총)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh1P;

public:
	// 총기 스펙 (사거리, 데미지 등)
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float FireRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Damage = 20.0f;

	// Getter
	USkeletalMeshComponent* GetWeaponMesh3P() const { return WeaponMesh3P; }
	USkeletalMeshComponent* GetWeaponMesh1P() const { return WeaponMesh1P; }
};
