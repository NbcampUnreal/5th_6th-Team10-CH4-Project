// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gimmick/Interfaces/Interactable.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameMode/GameTypes/GameTypes.h"
#include "FuseBoxActor.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AFuseBoxActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuseBoxActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//--------------------------------------------------------------

#pragma region Components - 컴포넌트
private:
	// 루트 컴포넌트로 사용할 구체 콜리전
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	// 시각적 표현을 위한 스태틱 메시
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> FuseBoxDoorMeshComp;

protected:
	UPROPERTY(EditAnywhere, Category = "Gimmick Settings")
	EGameArea BelongingArea;

public:
	EGameArea GetBelongingArea() const { return BelongingArea; }
#pragma endregion

//------------------------------------------------------------

#pragma region Interaction
public:
	virtual void Interact_Implementation(AActor* _Instigator) override;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated = false;

	UFUNCTION()
	void OnRep_IsActivated();	// 퓨즈가 꽂혔을 때
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion
//------------------------------------------------------------
#pragma region Interaction Text
public:
	virtual void SetInteractionUI_Implementation(bool bVisible, AActor* _Instigator) override;
	virtual FText GetInteractText_Implementation(AActor* _Instigator) const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UWidgetComponent> InteractionWidget;
#pragma endregion
};
