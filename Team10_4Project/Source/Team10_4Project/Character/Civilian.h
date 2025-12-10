// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "AbilitySystemInterface.h"
//#include "GameplayTagContainer.h"
//#include "InputActionValue.h" // UE 5.6 Enhanced Input
#include "Civilian.generated.h"

//class UAbilitySystemComponent;
//class USurvivalAbilitySystemComponent;
//class UCharacterAttributeSet;
//class UInteractionComponent;
//
//struct FOnAttributeChangeData;

UCLASS()
class TEAM10_4PROJECT_API ACivilian : public ACharacter
{
	GENERATED_BODY()

//public:
//	ACivilian();
    /*
	// IAbilitySystemInterface 구현
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// GAS 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class USurvivalAbilitySystemComponent* AbilitySystemComponent;

	// Attribute Set
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	class UCharacterAttributeSet* AttributeSet;

	// 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UInteractionComponent* InteractionComponent;

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    // GAS 초기화
    void InitializeAbilitySystem();
    void GiveDefaultAbilities();
    void ApplyDefaultEffects();

    // 기본 어빌리티 목록
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

    // 기본 효과 목록
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Effects")
    TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;

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

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Enhanced Input 콜백 함수들
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartJump();
    void StopJump();

    // 상호작용
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void TryInteract();

    // 공격
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryAttack();

    // 아이템 사용
    UFUNCTION(BlueprintCallable, Category = "Item")
    void TryUseItem(int32 ItemSlot);

    // Attribute 변경 콜백
    UFUNCTION()
    void OnHealthChanged(const struct FOnAttributeChangeData& Data);

    UFUNCTION()
    void OnStaminaChanged(const struct FOnAttributeChangeData& Data);

    // 사망 처리
    UFUNCTION(BlueprintImplementableEvent, Category = "Character")
    void OnDeath();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastHandleDeath();
    */
};
