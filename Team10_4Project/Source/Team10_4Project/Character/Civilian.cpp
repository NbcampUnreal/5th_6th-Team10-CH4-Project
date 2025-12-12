// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Civilian.h"

#include "AbilitySystemComponent.h"
#include "CivilianAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"

ACivilian::ACivilian()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); // ASC 상태 복제
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // ReplicationMode 설정
	
	AttributeSet = CreateDefaultSubobject<UCivilianAttributeSet>("AttributeSet");
	
	// 네트워크 설정
	SetReplicates(true); 
	SetReplicateMovement(true);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->SetIsReplicated(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

UAbilitySystemComponent* ACivilian::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACivilian::BeginPlay()
{
	Super::BeginPlay();
	
	// Attribute 변경 델리게이트 바인딩
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetHealthAttribute()).AddUObject(this, &ACivilian::OnHealthChanged);
	}
}

void ACivilian::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// 서버에서만 실행
	if (HasAuthority())
	{
		InitializeAbilitySystem();
	}
}

void ACivilian::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// UE 5.6: Enhanced Input Subsystem 등록
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &ACivilian::Move);

		// 시점
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &ACivilian::Look);

		// 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started,
			this, &ACivilian::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &ACivilian::StopJump);

		/*// 상호작용
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,
			this, &ACivilian::TryInteract);*/

		// 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started,
			this, &ACivilian::TryAttack);
	}
}

void ACivilian::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// 클라이언트에서 실행
	InitializeAbilitySystem();
}

void ACivilian::InitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
		return;
	
	
}

void ACivilian::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACivilian::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACivilian::StartJump()
{
	Jump();
}

void ACivilian::StopJump()
{
	StopJumping();
}

void ACivilian::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	// UI 업데이트 
	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f -> %f"), Data.OldValue, Data.NewValue);

	// 사망 체크
	if (Data.NewValue <= 0.0f && Data.OldValue > 0.0f)
	{
		MulticastHandleDeath();
	}
}

void ACivilian::TryAttack()
{
	if (!AbilitySystemComponent)
		return;
	
	// 공격 어빌리티 활성화 (GameplayTag 사용하여 구현할 예정)
}

void ACivilian::MulticastHandleDeath_Implementation()
{
	// 모든 클라이언트에서 실행
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 사망 애니메이션/이펙트
	OnDeath();
}
