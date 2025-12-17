// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
// #include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "InputActionValue.h" // UE 5.6 Enhanced Input
#include "Civilian.generated.h"

class UAbilitySystemComponent;
class UCivilianAttributeSet;
class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;

UCLASS()
class TEAM10_4PROJECT_API ACivilian : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region Civilian Override

public:
	ACivilian();
    
	// IAbilitySystemInterface 구현
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// 초기화
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_PlayerState() override;
	
#pragma endregion
	
#pragma region Civilian Components
public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/*FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }*/
	
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCamera; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

#pragma endregion
	
#pragma region Civilian GAS
	
public:
	// GAS 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// Attribute Set
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCivilianAttributeSet> AttributeSet;

	// 기본 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	// 기본 효과 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Effects")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

protected:
	// GAS 초기화 - ASC 초기화 로직 분리
	void InitializeAbilitySystem();
	void GiveDefaultAbilities();
	void ApplyDefaultEffects();
	
public:
	// Attribute 변경 콜백
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);

#pragma endregion
	
#pragma region Civilian Input
	
public:
	// Enhanced Input 콜백 함수들
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	
	// 공격
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Morph();
	
	// 공격
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryAttack();
	
	// float GetCurrentAimPitch() const { return CurrentAimPitch; }
	
protected:
	// Enhanced Input (UE 5.6)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MorphAction;
	
	/*UPROPERTY(Replicated)
	float CurrentAimPitch = 0.f;

	float PreviousAimPitch = 0.f;*/
	
#pragma endregion
	
	// 사망 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath();
	
		/*
    // 상호작용
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void TryInteract();

    // 아이템 사용
    UFUNCTION(BlueprintCallable, Category = "Item")
    void TryUseItem(int32 ItemSlot)
    */
};
