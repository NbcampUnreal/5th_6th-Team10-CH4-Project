// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
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
class UInGameUIWidget;
class UWidgetComponent;

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
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
#pragma endregion
	
#pragma region Civilian Components
public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	
	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMeshComponent; }
	
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCamera; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> WerewolfCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	// 에디터에서 WBP_InGameUI를 할당하기 위한 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInGameUIWidget> InGameUIClass;
	// 생성된 UI 인스턴스 저장용
	UPROPERTY()
	TObjectPtr<UInGameUIWidget> InGameUIInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> VoteWidgetComponent;

#pragma endregion
	
#pragma region Civilian GAS
	
public:
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TWeakObjectPtr<UCivilianAttributeSet> AttributeSet;

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
	
	virtual void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);

#pragma endregion
	
#pragma region Civilian Input
	
public:
	// 입력 바인딩 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void ActivateAbility(const FGameplayTag& AbilityTag);
	
	// 공격
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryAttack();
	
	// 태그 변경 시 호출될 콜백 함수
	virtual void OnGameplayTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
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
	
#pragma endregion
	
#pragma region Civilian Morph
	
public:
	// 변신
	UFUNCTION(BlueprintCallable, Category = "GAS|Morph")
	void Morph();

	// 멀티캐스트 RPC - 비쥬얼 변경 및 동기화
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMorph(bool bToInfected);
	
protected:
	// 변신 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Infected")
	class UInputAction* MorphAction;
	
	// 감염자 변신 시, 3인칭 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> MorphMesh;
	
	// 감염자 변신 시, 3인칭 ABP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components")
	TSubclassOf<UAnimInstance> MorphAnimClass;
	
	// 감염자 변신 시, 1인칭 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components")
	TObjectPtr<USkeletalMesh> MorphFirstPersonMesh;
	
	// 감염자 변신 시, 1인칭 ABP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCharacter|Components")
	TSubclassOf<UAnimInstance> MorphFirstPersonAnimClass;
	
	// 감염자 변신 시, 가릴 Mesh BonesS
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerCharacter|Components|MorphSettings")
	TArray<FName> MorphFirstPersonBonesToHide;
	
	// 변신 전, 상태 백업
	UPROPERTY()
	TObjectPtr<USkeletalMesh> DefaultThirdPersonMesh;
    
	UPROPERTY()
	TObjectPtr<USkeletalMesh> DefaultFirstPersonMesh;
	
	UPROPERTY()
	TSubclassOf<UAnimInstance> DefaultThirdPersonAnimClass;
	
	UPROPERTY()
	TSubclassOf<UAnimInstance> DefaultFirstPersonAnimClass;
	
public:
	// [테스트용] 콘솔 명령어 (Exec)
	// 사용법: ~ 키 누르고 "Cheat_SetRole 1" (1=Infected, 0=Civilian)
	UFUNCTION(Exec) 
	void Cheat_SetRole(int32 RoleID);

	// 사용법: ~ 키 누르고 "Cheat_SetSanity 100"
	UFUNCTION(Exec)
	void Cheat_SetSanity(float Amount);

protected:
	// 치트는 반드시 서버에서 실행
	UFUNCTION(Server, Reliable)
	void Server_SetRole(int32 RoleID);

	UFUNCTION(Server, Reliable)
	void Server_SetSanity(float Amount);
	
#pragma endregion
	
#pragma region Civilian Death
	
public:
	// AttributeSet에서 호출할 함수
	virtual void HandleFatalDamage(AActor* Attacker, bool bAttackerIsTransformed);
	
	UFUNCTION(BlueprintCallable, Category = "GAS|Respawn")
	void TeleportToSpawnLocation();

protected:
	// 스폰 위치 저장
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	FVector InitialSpawnLocation;
	
	// 사망 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
	void OnDeath();

	UFUNCTION(BlueprintCallable, Category = "GAS|Death")
	void MulticastHandleDeath();
#pragma endregion
	
#pragma region Civilian Weapon
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Slot1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Slot2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* IA_Reload;
	
	// 무기 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AWeaponBase> StartingWeaponClass;

	// 현재 들고 있는 무기
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, BlueprintReadOnly, Category = "Combat")
	class AWeaponBase* CurrentWeapon;
	
	UFUNCTION()
	void OnRep_CurrentWeapon(class AWeaponBase* OldWeapon);
	
	// Crosshair
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;
	
	// Crosshair 위젯
	UPROPERTY()
	class UUserWidget* CrosshairWidget;

	// 크로스헤어 보이기/숨기기 함수
	void UpdateCrosshairVisibility();
	
	// 무기 장착 해제 (1번 키)
	void UnEquipWeapon();

	// 특정 무기 클래스 장착 (2번 키 공용)
	void EquipWeapon(TSubclassOf<class AWeaponBase> NewWeaponClass);
	
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(TSubclassOf<class AWeaponBase> NewWeaponClass);

	UFUNCTION(Server, Reliable)
	void Server_UnEquipWeapon();
	
protected:
	// 입력 바인딩 함수
	void OnInput_Slot1(); // 비무장 전환
	void OnInput_Slot2(); // 권총 전환
	void OnInput_Reload(); // 장전
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	float PlayMontage1P(UAnimMontage* MontageToPlay);
	// Getter
	AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }
	
#pragma endregion
	
#pragma region Test
public:
	// [테스트용] 리스폰 로직 테스트 명령어
	UFUNCTION(Exec)
	void Cheat_TestRespawn();

protected:
	// 서버에게 리스폰을 요청하는 RPC
	UFUNCTION(Server, Reliable)
	void Server_TestRespawn();
	
#pragma endregion
	
#pragma region Interaction Logic - 상호작용 로직
protected:
	// 클라이언트에서 서버로 상호작용을 요청하는 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_Interact(AActor* TargetActor);

	// 클라이언트에서 주변 Gimmick을 찾는 Line Trace
	void Client_PerformInteractTrace();

	// 입력 매핑 호출 함수 (예: E키)
	void InteractInputPressed();

	// 상호작용 가능 액터가 존재하는지 확인하는 함수
	AActor* GetInteractableActor();

private:
	// 현재 상호작용 중인 액터 (Line Trace의 결과 값)
	TWeakObjectPtr<AActor> CurrentInteractableActor;

	UPROPERTY()
	TObjectPtr<AActor> LastLookedInteractable;

	// 상호작용 거리
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractDistance = 200.0f;
#pragma endregion

#pragma region Inventory Logic - 인벤토리 로직
public:
	// [테스트용]
	UFUNCTION(Exec)
	void Cheat_AddItem(FName ItemID);
	UFUNCTION(Exec)
	void Cheat_AddStackItem(FName ItemID, int32 Count);
protected:
	// [테스트용]
	UFUNCTION(Server, Reliable)
	void Server_AddItem(FName ItemID);
	UFUNCTION(Server, Reliable)
	void Server_AddStackItem(FName ItemID, int32 Count);

#pragma endregion

#pragma region Vote
public:
	UFUNCTION()
	void VoteWidgetActive(const FGameplayTag CallbackTag, int32 NewCount);
#pragma endregion

};
