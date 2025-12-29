// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Civilian.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/GamePlayTags.h"
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
#include "Gimmick/Interfaces/Interactable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon/WeaponBase.h"


ACivilian::ACivilian()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// 네트워크 설정
	SetReplicates(true); 
	SetReplicateMovement(true);
	
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
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
	FirstPersonMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	FirstPersonMeshComponent->bCastDynamicShadow = false; // 1인칭 Mesh는 그림자가 안보이도록 설정
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->SetRelativeLocation(FVector(-30.0f, 0.0f, -150.0f));
	FirstPersonMeshComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	
	// 변신 시, 1인칭 메쉬 본 숨기기 목록
	MorphFirstPersonBonesToHide = {
		FName("neck_01"),
		FName("head")
	};
}

UAbilitySystemComponent* ACivilian::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACivilian::BeginPlay()
{
	Super::BeginPlay();
	
	InitialSpawnLocation = GetActorLocation(); // 처음 시작 시 스폰 위치 저장
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	// Attribute 변경 델리게이트 바인딩
	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetHealthAttribute()).AddUObject(this, &ACivilian::OnHealthChanged);
		
		ASC->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMoveSpeedAttribute()).AddUObject(this, &ACivilian::OnMoveSpeedChanged);
		
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
		}
	}
	
	// 3인칭 (Full Body) 백업
	if (GetMesh())
	{
		// 메쉬 에셋 저장
		DefaultThirdPersonMesh = GetMesh()->GetSkeletalMeshAsset();
		
		// 애니메이션 블루프린트 클래스 저장
		DefaultThirdPersonAnimClass = GetMesh()->GetAnimClass();
	}

	// 1인칭 (Arms) 백업
	if (FirstPersonMeshComponent) 
	{
		// 메쉬 에셋 저장 
		DefaultFirstPersonMesh = FirstPersonMeshComponent->GetSkeletalMeshAsset();

		// 애니메이션 블루프린트 클래스 저장
		DefaultFirstPersonAnimClass = FirstPersonMeshComponent->GetAnimClass();
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

void ACivilian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 로컬 플레이어인 경우에만 상호작용 문구 UI 제어
	if (IsLocallyControlled())
	{
		//현재 조준 중인 액터 찾기
		AActor* HitActor = GetInteractableActor();

		// 바라보는 대상이 바뀌었을 때만 실행
		if (LastLookedInteractable != HitActor)
		{
			// 이전에 보던 액터가 있고, 인터페이스를 구현했다면 UI 끄기
			if (LastLookedInteractable && LastLookedInteractable->Implements<UInteractable>())
			{
				IInteractable::Execute_SetInteractionUI(LastLookedInteractable, false, this);
			}

			// 새로 보는 액터가 있고, 인터페이스를 구현했다면 UI 켜기
			if (HitActor && HitActor->Implements<UInteractable>())
			{
				IInteractable::Execute_SetInteractionUI(HitActor, true, this);
			}

			// 상태 갱신
			LastLookedInteractable = HitActor;
		}

		// 기존 CurrentInteractableActor 변수 업데이트 (필요 시)
		CurrentInteractableActor = HitActor;
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

		// 상호작용
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started,
			this, &ACivilian::InteractInputPressed);

		// 공격
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started,
			this, &ACivilian::TryAttack);
		
		// 변신
		EnhancedInputComponent->BindAction(MorphAction, ETriggerEvent::Started,
			this, &ACivilian::Morph);
		
		if (IA_Slot1)
		{
			EnhancedInputComponent->BindAction(IA_Slot1, ETriggerEvent::Started,
				this, &ACivilian::OnInput_Slot1);
		}
		if (IA_Slot2)
		{
			EnhancedInputComponent->BindAction(IA_Slot2, ETriggerEvent::Started,
				this, &ACivilian::OnInput_Slot2);
		}
	}
}

void ACivilian::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// 클라이언트에서 실행
	InitializeAbilitySystem();
}

void ACivilian::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACivilian, CurrentWeapon);
}

float ACivilian::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	// 3인칭 메쉬(Body) 애니메이션 재생 및 서버 동기화 처리
	float Duration = Super::PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);

	// [로컬 플레이어 전용] 1인칭 메쉬에도 애니메이션 재생
	if (IsLocallyControlled() && FirstPersonMeshComponent)
	{
		UAnimInstance* AnimInstance = FirstPersonMeshComponent->GetAnimInstance();
		if (AnimInstance && AnimMontage)
		{
			UE_LOG(LogTemp, Warning, TEXT("[FPP] Playing Montage on FPP Mesh: %s"), *AnimMontage->GetName());
			AnimInstance->Montage_Play(AnimMontage, InPlayRate);
            
			// 섹션 점프가 필요하다면
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[FPP] AnimInstance is NULL!"));
		}
	}

	return Duration;
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
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	if (!HasAuthority() || !AbilitySystemComponent.IsValid()) return;
	
	for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Ability, 1, -1, this);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
	
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
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// 시민 스턴이거나 감염자 스턴이면 -> 이동 입력 무시 및 리턴
		if (ASC->HasMatchingGameplayTag(GamePlayTags::CivilianState::Stun) ||
			ASC->HasMatchingGameplayTag(GamePlayTags::InfectedState::Stun))
		{
			return; 
		}
	}
	
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
	// 스턴 상태면 점프 불가
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ASC->HasMatchingGameplayTag(GamePlayTags::CivilianState::Stun) ||
			ASC->HasMatchingGameplayTag(GamePlayTags::InfectedState::Stun))
		{
			return;
		}
	}
	
	Jump();
}

void ACivilian::StopJump()
{
	StopJumping();
}

void ACivilian::ActivateAbility(const FGameplayTag& AbilityTag)
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
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

void ACivilian::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
		
		UE_LOG(LogTemp, Log, TEXT("Speed Updated: %f"), Data.NewValue);
	}
}

void ACivilian::Morph()
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;
	
	UE_LOG(LogTemp, Log, TEXT("Morph Input -> Try Activate Ability"));
	
	// 태그 컨테이너 생성 및 태그로 어빌리티 실행
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(GamePlayTags::Ability::Infected::Morph);
    
	ASC->TryActivateAbilitiesByTag(TagContainer);
}

void ACivilian::Cheat_SetRole(int32 RoleID)
{
	// 서버 RPC 호출
	Server_SetRole(RoleID);
}

void ACivilian::Cheat_SetSanity(float Amount)
{
	Server_SetSanity(Amount);
}

void ACivilian::HandleFatalDamage(AActor* Attacker, bool bAttackerIsTransformed)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	// 내가 변신한 괴물인가?
	if (ASC->HasMatchingGameplayTag(GamePlayTags::InfectedState::Transformed))
	{
		// [CASE A] 감염자 리스폰 (Infected Stun)
		UE_LOG(LogTemp, Warning, TEXT("Infected Groggy....."));
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(GamePlayTags::Ability::Infected::Stun); // 태그 새로 정의 필요
		ASC->TryActivateAbilitiesByTag(TagContainer);
	}
	else
	{
		// 인간 폼
		if (bAttackerIsTransformed)
		{
			// [CASE B] 즉사 (Death)
			UE_LOG(LogTemp, Warning, TEXT("Player Dead."));
			MulticastHandleDeath();
		}
		else
		{
			// [CASE C] 시민 투표 대기 (Civilian Voting)
			UE_LOG(LogTemp, Warning, TEXT("Start Voting....."));
			
			/*FGameplayTagContainer TagContainer;
			TagContainer.AddTag(GamePlayTags::Ability::Civilian::Stun); // 태그 새로 정의 필요
			ASC->TryActivateAbilitiesByTag(TagContainer);*/
		}
	}
}

void ACivilian::TeleportToSpawnLocation()
{
	// 서버 권한 확인 
	if (HasAuthority())
	{
		// 물리/이동 정지 (안전 장치)
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
		}

		// 위치 이동
		SetActorLocation(InitialSpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
        
		// 회전값 초기화
		SetActorRotation(FRotator::ZeroRotator);
        
		UE_LOG(LogTemp, Log, TEXT("Respawned at: %s"), *InitialSpawnLocation.ToString());
	}
}

void ACivilian::MulticastHandleDeath()
{
	// 중복 실행 방지 (이미 죽었는데 또 죽는 것 방지)
	if (GetMesh()->IsSimulatingPhysics()) return;

	// 어빌리티 시스템 정리 (사용 중인 스킬 취소)
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->CancelAllAbilities();
		// 사망 상태 태그 추가 (로직 안전장치)
		ASC->AddLooseGameplayTag(GamePlayTags::CivilianState::Dead);
	}

	// 컨트롤러 입력 차단
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
        
		// (선택사항) 데스 캠이나 관전 모드 전환 로직은 BP_OnDeath에서 처리 추천
	}

	// 무브먼트 컴포넌트 비활성화 (이동 불가)
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->SetComponentTickEnabled(false); // 성능 최적화
	}

	// 캡슐 콜리전 비활성화
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	// 3인칭 메쉬: 래그돌(Ragdoll) 전환
	if (GetMesh())
	{
		// 캡슐이 꺼졌으므로 메쉬가 물리 충돌을 대신해야 함
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll")); 
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bPauseAnims = true; // 애니메이션 정지 (물리에 맡김)
		
	}

	// 1인칭 메쉬(팔) 숨기기
	if (FirstPersonMeshComponent)
	{
		FirstPersonMeshComponent->SetHiddenInGame(true);
	}

	// 블루프린트 이벤트 호출 (사망 UI, 사운드, 파티클 등)
	OnDeath();
}

void ACivilian::OnRep_CurrentWeapon(class AWeaponBase* OldWeapon)
{
	// 1. 기존 무기가 있었다면 제거 (혹은 숨김)
	if (OldWeapon)
	{
		OldWeapon->Destroy(); // 혹은 DetachFromActor 등
	}

	// 2. 새 무기가 들어왔다면 손에 부착!
	if (CurrentWeapon)
	{
		// 3인칭 메쉬에 부착 (다른 사람이 볼 때 중요)
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));

		// [중요] 만약 '내 화면(IsLocallyControlled)'이라면 1인칭 팔에도 붙여줘야 함
		if (IsLocallyControlled() && FirstPersonMeshComponent) 
		{
			if (USkeletalMeshComponent* WeaponMesh1P = CurrentWeapon->GetWeaponMesh1P())
			{
				WeaponMesh1P->AttachToComponent(FirstPersonMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
			}
		}
	}
}

void ACivilian::Server_EquipWeapon_Implementation(TSubclassOf<class AWeaponBase> NewWeaponClass)
{
	EquipWeapon(NewWeaponClass);
}

void ACivilian::Server_UnEquipWeapon_Implementation()
{
	UnEquipWeapon();
}

void ACivilian::UnEquipWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy(); // 액터 파괴 (1인칭/3인칭 메쉬 모두 사라짐)
		CurrentWeapon = nullptr;
        
		UE_LOG(LogTemp, Log, TEXT("Weapon Unequipped (Unarmed Mode)"));
	}
}

void ACivilian::EquipWeapon(TSubclassOf<class AWeaponBase> NewWeaponClass)
{
	if (!NewWeaponClass || !GetWorld()) return;

	// 서버 권한 확인 (멀티플레이라면 서버 RPC로 처리해야 함. 싱글이면 바로 실행)
	if (!HasAuthority()) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	// 무기 생성
	AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(NewWeaponClass, GetActorLocation(), GetActorRotation(), Params);

	if (NewWeapon)
	{
		// 3인칭 부착
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));

		CurrentWeapon = NewWeapon;
		UE_LOG(LogTemp, Log, TEXT("Pistol Equipped!"));
	}
}

void ACivilian::OnInput_Slot1()
{
	UE_LOG(LogTemp, Log, TEXT("Input Slot 1"));
	
	if (HasAuthority()) UnEquipWeapon();
	else Server_UnEquipWeapon();
}

void ACivilian::OnInput_Slot2()
{
	UE_LOG(LogTemp, Log, TEXT("Input Slot 2"));
	
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;
	
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid()) return;
	
	if (AbilitySystemComponent->HasMatchingGameplayTag(GamePlayTags::InfectedState::Transformed))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot equip weapon while transformed!"));
		return; 
	}
	
	// 이미 권총을 들고 있거나, 설정된 권총 클래스가 없으면 무시
	if (!StartingWeaponClass) return;
	if (CurrentWeapon && CurrentWeapon->IsA(StartingWeaponClass)) return;

	if (HasAuthority())
	{
		// 내가 서버라면 바로 실행
		if (CurrentWeapon) UnEquipWeapon();
		EquipWeapon(StartingWeaponClass);
	}
	else
	{
		// 클라이언트라면 서버에게 부탁 (RPC)
		Server_UnEquipWeapon(); // 기존 무기 해제 요청
		Server_EquipWeapon(StartingWeaponClass); // 권총 장착 요청
	}
}

void ACivilian::Server_SetRole_Implementation(int32 RoleID)
{
	if (ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>())
	{
		// 0: Civilian, 1: Infected
		FGameplayTag NewTag = (RoleID == 1) ? 
			GamePlayTags::PlayerRole::Infected : 
			GamePlayTags::PlayerRole::Civilian;
            
		PS->SetPlayerRoleTag(NewTag);
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

void ACivilian::MulticastMorph_Implementation(bool bToInfected)
{
	if (bToInfected)
	{
		// 변신 전 들고 있는 무기가 있다면 강제 해제
		UnEquipWeapon();
		
		if (MorphMesh && GetMesh())
		{
			// 3인칭 메쉬 교체
			GetMesh()->SetSkeletalMesh(MorphMesh);

			// 3인칭 ABP 교체 
			if (MorphAnimClass)
			{
				GetMesh()->SetAnimInstanceClass(MorphAnimClass);
			}
		
			// 1인칭 메쉬 교체
			if (IsLocallyControlled() && FirstPersonMeshComponent)
			{
				if (MorphFirstPersonMesh)
				{
					// 1인칭 메쉬 교체
					FirstPersonMeshComponent->SetSkeletalMesh(MorphFirstPersonMesh);
				
					// 1인칭 ABP 교체 
					if (MorphFirstPersonAnimClass)
					{
						FirstPersonMeshComponent->SetAnimInstanceClass(MorphFirstPersonAnimClass);
					}
				
					// 필요없는 본 숨기기
					for (const FName& BoneName : MorphFirstPersonBonesToHide)
					{
						// PBO_None: 물리에는 영향을 주지 않고 렌더링만 숨김
						FirstPersonMeshComponent->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);
					}
            
					// 변경 사항 즉시 반영을 위해 렌더 상태 갱신
					FirstPersonMeshComponent->MarkRenderStateDirty();
				}
			}
		
			UE_LOG(LogTemp, Log, TEXT("Morph Complete!"));
		}
	}
	else
	{
		// 3인칭 복구
		if (GetMesh())
		{
			if (DefaultThirdPersonMesh)
			{
				GetMesh()->SetSkeletalMesh(DefaultThirdPersonMesh);
			}
			
			if (DefaultThirdPersonAnimClass)
			{
				GetMesh()->SetAnimInstanceClass(DefaultThirdPersonAnimClass);
			}
		}

		// 1인칭 복구 (로컬 플레이어만)
		if (IsLocallyControlled() && FirstPersonMeshComponent)
		{
			if (DefaultFirstPersonMesh)
			{
				FirstPersonMeshComponent->SetSkeletalMesh(DefaultFirstPersonMesh);
			}

			if (DefaultFirstPersonAnimClass)
			{
				FirstPersonMeshComponent->SetAnimInstanceClass(DefaultFirstPersonAnimClass);
			}
            
			// 렌더 상태 갱신
			FirstPersonMeshComponent->MarkRenderStateDirty();
		}
	}
	
}

void ACivilian::TryAttack()
{
	ACivilianPlayerState* PS = GetPlayerState<ACivilianPlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTag AbilityTagToActivate;

	// 현재 플레이어 역할 확인 (태그 기반)
	bool bIsMonsterForm = ASC->HasMatchingGameplayTag(GamePlayTags::InfectedState::Transformed);

	if (bIsMonsterForm)
	{
		// [상태: 괴물] 무조건 감염자 전용 근접 공격 실행
		AbilityTagToActivate = GamePlayTags::InfectedAbility::Primary;
        
		UE_LOG(LogTemp, Log, TEXT("[Attack] Monster Form - Melee Attack"));
	}
	else
	{
		// [상태: 인간] (시민이거나, 인간으로 위장한 감염자)
        
		// 무기를 들고 있는지 확인 (CurrentWeapon이 null이 아니면 총을 든 것)
		if (CurrentWeapon)
		{
			// 무기 있음 -> 사격 어빌리티 실행
			AbilityTagToActivate = GamePlayTags::CivilianAbility::Shoot;
			UE_LOG(LogTemp, Log, TEXT("[Attack] Human Form - Fire Gun (Shoot)"));
		}
		else
		{
			// 무기 없음 -> 맨손 공격 실행
			AbilityTagToActivate = GamePlayTags::CivilianAbility::Primary;
			UE_LOG(LogTemp, Log, TEXT("[Attack] Human Form - Punch (Primary)"));
		}
	}

	// 3. 결정된 어빌리티 실행 요청
	if (AbilityTagToActivate.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTagToActivate));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] Invalid Ability Tag!"));
	}
}

#pragma region Interaction Logics - 상호작용 로직 구현
// E 키 입력 처리
void ACivilian::InteractInputPressed()
{
	if (!HasAuthority() && !IsLocallyControlled()) return;

	// 소유권 확인 로그
	UE_LOG(LogTemp, Warning, TEXT("Local Owner: %s"), GetOwner() ? *GetOwner()->GetName() : TEXT("No Owner"));

	UE_LOG(LogTemp, Warning, TEXT("Interact Key Pressed!"));
	AActor* TargetActor = GetInteractableActor(); // 이 줄은 E 키 눌렀을 때 실행이 아니라, Tick() 에서 상시 실행 가능성도 있음.

	if (TargetActor)
	{
		//if (HasAuthority())
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("I am Server, Executing Directly"));
		//	IInteractable::Execute_Interact(TargetActor, this);
		//}
		//else
		//{
		//	// 내가 클라이언트라면 서버에 요청
		//	UE_LOG(LogTemp, Warning, TEXT("I am Client, Sending RPC"));
		//	ServerRPC_Interact(TargetActor);
		//}
		UE_LOG(LogTemp, Warning, TEXT("Target Found: %s. Calling ServerRPC_Interact"), *TargetActor->GetName());
		ServerRPC_Interact(TargetActor);
	}
}

// RPC 구현 (클라이언트 -> 서버)
void ACivilian::ServerRPC_Interact_Implementation(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Server: received interact request for Target: %s"), *TargetActor->GetName());

	if (!TargetActor)
	{
		return;
	}

	float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());	// 거리가 너무 멀지는 않은지 서버에서도 한 번 더 체크 (핵 방지용)
	{
		if (Distance > InteractDistance + 100.0f) return;
	}

	if (TargetActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(TargetActor, this);
	}
}

// 상호작용 가능한 액터를 찾는 Line Trace (클라이언트 전용)
AActor* ACivilian::GetInteractableActor()
{
	UE_LOG(LogTemp, Warning, TEXT("Perform Trace"));

	// 1인칭 카메라 컴포넌트 사용
	if (!FirstPersonCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Camera is Null!"));
		return nullptr;
	}
	FVector StartLocation = FirstPersonCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + (FirstPersonCamera->GetForwardVector() * InteractDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 나 자신은 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(	// LineTraceSingleByChannel 사용 (성능상 더 가벼움)
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility, // 상호작용용 채널 (보통 Visibility 사용)
		Params
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->Implements<UInteractable>())
		{
			return HitActor;
		}
	}
	return nullptr;
}
#pragma endregion