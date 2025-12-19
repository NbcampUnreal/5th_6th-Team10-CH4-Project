// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gimmick/Interfaces/Interactable.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AmmoBoxActor.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AAmmoBoxActor : public AActor, public IInteractable // Interatable 인터페이스 상속
{
	GENERATED_BODY()

#pragma region Default Functions (AAmmoBoxActor(), BeginPlay(), Tick(float DeltaTime)
public:
	// Sets default values for this actor's properties
	AAmmoBoxActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma endregion


#pragma region Components - 컴포넌트
private:
	// 루트 컴포넌트로 사용할 구체 콜리전
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	// 시각적 표현을 위한 스태틱 메시
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;
#pragma endregion


#pragma region IInteractable Override
public:
	virtual void Interact_Implementation(AActor* _Instigator) override;
	virtual FText GetInteractText_Implementation() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> GEClassAmmoBox;
#pragma endregion

};
