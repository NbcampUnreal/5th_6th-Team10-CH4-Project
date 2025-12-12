// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "GimmickTestCharacter.generated.h"

class UAbilitySystemComponent;
class UCameraComponent;

UCLASS()
class TEAM10_4PROJECT_API AGimmickTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGimmickTestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// ============================================================================================================ //

#pragma region GAS 필수 구현
public:
	// IAbilitySystemInterface 인터페이스 함수 오버라이드
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	void InitializeAttributes();
#pragma endregion


#pragma region Components / 컴포넌트
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;	// GAS 컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;	// 1인칭 시점 카메라
#pragma endregion


#pragma region Interaction Logic - 상호작용 로직
protected:
	// 클라이언트에서 서버로 상호작용을 요청하는 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_Interact(AActor* TargetActor);

	// 클라이언트에서 주변 Gimmick을 찾는 Line Trace
	void Client_PerformInteractTrace();

	// 입력 매핑 (예: E키)
	void InteractInputPressed();

	// 상호작용 가능 액터가 존재하는지 확인하는 함수
	AActor* GetInteractableActor();

private:
	// 현재 상호작용 중인 액터 (Line Trace의 결과 값)
	TWeakObjectPtr<AActor> CurrentInteractableActor;

	// 상호작용 거리
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractDistance = 400.0f;
#pragma endregion


#pragma region EnhancedInput - 입력
protected:
	// 사용할 입력 액션(IA)들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Interact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move; // 이동 (XY 축)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look; // 시점 회전 (XY 축)

	// 캐릭터에 적용할 입력 매핑 컨텍스트(IMC)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_GimmickTest;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
#pragma endregion
};
