// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Civilian.h"

#include "AbilitySystemComponent.h"
#include "CivilianAttributeSet.h"
#include "Character/CivilianPlayerState.h"
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
	
	// 네트워크 설정
	SetReplicates(true); 
	SetReplicateMovement(true);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->SetIsReplicated(true);
	
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	if (GetMesh())
	{
		// 화면에 캐릭터가 보이든 안 보이든 상관없이 뼈대(애니메이션)위치를 항상 갱신
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		
		GetMesh()->SetOwnerNoSee(true); // 로컬(나)에게는 안보이게 설정
		GetMesh()->bCastHiddenShadow = true; // 3인칭 Mesh는 안보이지만 그림자는 보이도록 설정 
		
		// 3인칭 Mesh는 캡슐 안에서 위치 잡기
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
	FirstPersonCamera->bUsePawnControlRotation = true; // 마우스 회전에 따라 카메라 회전
	
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMeshComponent->SetupAttachment(FirstPersonCamera);
	FirstPersonMeshComponent->SetOnlyOwnerSee(true); // 로컬(나)에게만 보이도록 설정
	FirstPersonMeshComponent->bCastDynamicShadow = false; // 1인칭 Mesh는 그림자가 안보이도록 설정
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));
	FirstPersonMeshComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
}

UAbilitySystemComponent* ACivilian::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACivilian::BeginPlay()
{
	Super::BeginPlay();
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	// Attribute 변경 델리게이트 바인딩
	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
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
		
		// 변신
		EnhancedInputComponent->BindAction(MorphAction, ETriggerEvent::Started,
			this, &ACivilian::Morph);
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
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	AttributeSet = PS->GetAttributeSet();
	if (!AttributeSet.IsValid()) return;
	
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	
	if (HasAuthority())
	{
		GiveDefaultAbilities();
		ApplyDefaultEffects();
	}
}

void ACivilian::GiveDefaultAbilities()
{
}

void ACivilian::ApplyDefaultEffects()
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	if (!HasAuthority() || !AbilitySystemComponent.IsValid()) return;
	
	// 이펙트 컨테스트 생성
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
	{
		// 이펙트 적용
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
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

void ACivilian::Morph()
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Morphing!!"));
	
	ServerTryMorph();
}

void ACivilian::Cheat_SetRole(int32 RoleID)
{
	// 입력받은 정수를 Enum으로 변환 (0: Civilian, 1: Infected)
	EPlayerRole NewRole = (RoleID == 1) ? EPlayerRole::Infected : EPlayerRole::Civilian;
    
	// 서버 RPC 호출
	Server_SetRole(NewRole);
}

void ACivilian::Cheat_SetSanity(float Amount)
{
	Server_SetSanity(Amount);
}

void ACivilian::Server_SetRole_Implementation(EPlayerRole NewRole)
{
	if (ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>())
	{
		PS->SetPlayerRole(NewRole);
        
		// 로그 출력
		FString RoleName = (NewRole == EPlayerRole::Infected) ? TEXT("Infected") : TEXT("Civilian");
		UE_LOG(LogTemp, Warning, TEXT("[Cheat] Role Changed to: %s"), *RoleName);
	}
}

void ACivilian::Server_SetSanity_Implementation(float Amount)
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (PS)
	{
		UCivilianAttributeSet* AS = PS->GetAttributeSet();
		if (AS)
		{
			// AttributeSet의 Setter 매크로 사용
			AS->SetSanity(Amount);
            
			UE_LOG(LogTemp, Warning, TEXT("[Cheat] Sanity Set to: %f"), Amount);
		}
	}
}

void ACivilian::ServerTryMorph_Implementation()
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	// 역할 검증
	if (PS->GetPlayerRole() != EPlayerRole::Infected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Morph Failed: Not Infected"));
		return;
	}
	
	if (UCivilianAttributeSet* AS = PS->GetAttributeSet())
	{
		float CurrentSanity = AS->GetSanity();
		if (CurrentSanity < 100.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Morph Failed: Not enough Sanity (Current: %f)"), CurrentSanity);
			return;
		}
	}
	
	// 변신 개시
	MulticastMorph();
}

void ACivilian::MulticastMorph_Implementation()
{
	if (MorphMesh && GetMesh())
	{
		// 메쉬 교체
		GetMesh()->SetSkeletalMesh(MorphMesh);

		// ABP 교체 
		if (MorphAnimClass)
		{
			GetMesh()->SetAnimInstanceClass(MorphAnimClass);
		}

		// 이동 속도 증가 등 추가 로직
		
		UE_LOG(LogTemp, Log, TEXT("Morph Complete!"));
	}
}

void ACivilian::TryAttack()
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	// 공격 어빌리티 활성화 (GameplayTag 사용하여 구현할 예정)
	UE_LOG(LogTemp, Warning, TEXT("Attack!!"));
}

void ACivilian::MulticastHandleDeath_Implementation()
{
	// 모든 클라이언트에서 실행
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 사망 애니메이션/이펙트
	OnDeath();
}
