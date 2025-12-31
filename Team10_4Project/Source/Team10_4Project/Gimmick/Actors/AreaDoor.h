// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameMode/GameTypes/GameTypes.h"
#include "GamePlayTag/GamePlayTags.h"
#include "AreaDoor.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AAreaDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAreaDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//------------------------------------------------

#pragma region Components - 컴포넌트
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
#pragma endregion

//------------------------------------------------

#pragma region Settings - 설정
protected:
	// 이 문이 열려야 하는 구역 (예: Area 1->2 사이의 문이라면 Area 2 설정)
	/*UPROPERTY(EditAnywhere, Category = "Gimmick Settings")
	EGameArea TargetArea;*/
	UPROPERTY(EditAnywhere, Category = "Gimmick")
	FGameplayTag TargetAreaTag;

	UFUNCTION()
	void HandleAreaChanged(FGameplayTag NewAreaTag);
#pragma endregion

//------------------------------------------------

#pragma region States & Replication - 상태 및 복제
protected:
	UPROPERTY(ReplicatedUsing = OnRep_IsDoorOpen)
	bool bIsDoorOpen = false;

	UFUNCTION()
	void OnRep_IsDoorOpen();
#pragma endregion

//------------------------------------------------

#pragma region Event Binding - 이벤트 바인딩
protected:
	//UFUNCTION()
	//void HandleAreaChanged(EGameArea NewArea);

	UFUNCTION(BlueprintNativeEvent, Category = "Gimmick")
	void OpenDoor();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion
};
