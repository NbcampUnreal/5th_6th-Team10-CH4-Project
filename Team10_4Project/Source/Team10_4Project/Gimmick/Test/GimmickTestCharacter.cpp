// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Test/GimmickTestCharacter.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Gimmick/Interfaces/Interactable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "Gimmick/GAS/AS_Deceit.h"


#pragma region 기본 함수 (AGimmickTestCharacter() [ASC + 컴포넌트 생성 및 부착], BeginPlay(), Tick(float DeltaTime))
// Sets default values
AGimmickTestCharacter::AGimmickTestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ASC 생성 및 부착 (GAS필수)
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 카메라 컴포넌트 생성 및 부착
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(GetMesh(), FName("head")); // 캐릭터 Head 소켓에 부착
	CameraComp->bUsePawnControlRotation = true;

}

// Called when the game starts or when spawned
void AGimmickTestCharacter::BeginPlay()
{
	Super::BeginPlay();

    // 서버에서만 실행됩니다.
    // InitAbilityActorInfo는 오직 서버에서만 호출되어야 하며,
    // 클라이언트의 경우 복제(Replication)를 통해 자동으로 동기화됩니다.
    if (HasAuthority() && ASC)
    {
        // Owner는 PlayerState, Avatar는 Character 자신
        if (APlayerState* PS = GetPlayerState())
        {
            // 이 시점에서는 PlayerState가 거의 확실하게 유효합니다.
            ASC->InitAbilityActorInfo(PS, this);

            // ASC 설정 직후, Attribute Set을 명시적으로 적용 (타이밍 문제 해결)
            InitializeAttributes();
        }
    }
}

// Attribute Set을 명시적으로 생성하고 ASC에 적용
void AGimmickTestCharacter::InitializeAttributes()
{
    // C++ 클래스인 UAS_Deceit를 찾아서 ASC에 인스턴스화하여 추가합니다.
    if (ASC && UAS_Deceit::StaticClass())
    {
        if (!ASC->GetSet<UAS_Deceit>())
        {
            // UAS_Deceit::StaticClass()를 기반으로 Attribute Set 인스턴스를 생성하고 ASC에 등록합니다.
            // NewObject 대신 FGameplayEffectContext를 사용하는 표준 방식이 더 안정적입니다.

            // ⭐️⭐️ 임시 조치: 블루프린트 에셋 없이 기본 속성을 즉시 적용 ⭐️⭐️
            // *주의: 이 코드를 사용하려면, AttributeSet이 DefaultObject (CDO)에 정의한 값이 0이 아니어야 합니다.*
            ASC->AddSet<UAS_Deceit>();
        }

        // 로그 추가 (디버깅 완료 후 제거해도 됨)
        if (const UAS_Deceit* DeceitAS = ASC->GetSet<UAS_Deceit>())
        {
            UE_LOG(LogTemp, Warning, TEXT("Server: Attribute Set UAS_Deceit successfully created and initialized."));
        }
    }
}

// Called every frame
void AGimmickTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion

UAbilitySystemComponent* AGimmickTestCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

#pragma region Interaction Logics - 상호작용 로직 구현
// E 키 입력 처리
void AGimmickTestCharacter::InteractInputPressed()
{
	// Line Trace로 상호작용 가능한 액터를 찾는다.
	// ^ 실제 게임에서는 Tick()에서 실시간으로 Line Trace 하고있을 수도 있음. (그래야 상호작용 대상에게 마우스 올렸을 시 "Press E to Interact" 문구가 자동으로 생성될 것.)
	//	^ 이런 경우, Tick()에 있는 Line Trace로 Interactable 액터는 이미 저장되어있으며, E를 눌러 InteractInputPressed() 호출 시 그냥 ServerRPC_Interact()만 실행 할 수도 있음.
	AActor* TargetActor = GetInteractableActor(); // <-- 이 줄은 E 키 눌렀을 때 실행이 아니라, Tick() 에서 상시 실행 가능성도 있음.

	if (TargetActor)
	{
		// TargetActor가 존재한다면, 그 액터와의 상호작용 시도를 서버에게 요청 (서버 RPC)
		ServerRPC_Interact(TargetActor);
	}
}

// RPC 구현 (클라이언트 -> 서버)
void AGimmickTestCharacter::ServerRPC_Interact_Implementation(AActor* TargetActor)
{
	// 이 코드는 서버에서 실행
    UE_LOG(LogTemp, Warning, TEXT("Server: received interact request for Target: %s"), *TargetActor->GetName());

	// TargetActor가 여전히 유효한지 재차 검증
	if (!TargetActor)
	{
		return;
	}

	// TargetActor가 Interactable 인터페이스를 구현/상속했는지 확인.
	if (TargetActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(TargetActor, this);
	}
}

// 상호작용 가능한 액터를 찾는 Line Trace (클라이언트 전용)
AActor* AGimmickTestCharacter::GetInteractableActor()
{
	APlayerController* PC = GetController<APlayerController>();
	if (!PC)
	{
		return nullptr;
	}

	FVector StartLocation = CameraComp->GetComponentLocation(); // Trace Line 의 시작점
	FVector EndLocation = StartLocation + CameraComp->GetForwardVector() * InteractDistance; // Trace Line 의 종점

	FHitResult HitResult;
	// Line Trace 로직 (Line Trace Single By Channel 사용)
	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		StartLocation,
		EndLocation,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForDuration, // 디버그 드로우
		HitResult,
		true
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->Implements <UInteractable>())
		{
			return HitActor;
		}
	}
	return nullptr;
}
#pragma endregion


#pragma region Enhanced Input - 입력
// Called to bind functionality to input
void AGimmickTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

// 1. Enhanced Input Component 가져오기
    UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    
    if (EnhancedInputComp && GetController() && IMC_GimmickTest)
    {
        // 2. 입력 서브시스템에 IMC 등록 (Local Player만)
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (ULocalPlayer* LP = PC->GetLocalPlayer())
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
                {
                    Subsystem->AddMappingContext(IMC_GimmickTest, 0); // 0은 우선순위
                }
            }
        }
        
        // 3. IA_Interact 바인딩 (트리거: Pressed)
        if (IA_Interact)
        {
            EnhancedInputComp->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AGimmickTestCharacter::InteractInputPressed);
        }

        // 4. IA_Move 바인딩 (이동)
        if (IA_Move)
        {
            // FVector2D (X, Y) 입력을 처리할 새로운 C++ 함수가 필요합니다.
            EnhancedInputComp->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AGimmickTestCharacter::Move);
        }

        // 5. IA_Look 바인딩 (시점 회전)
        if (IA_Look)
        {
            // FVector2D (X, Y) 입력을 처리할 새로운 C++ 함수가 필요합니다.
            EnhancedInputComp->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AGimmickTestCharacter::Look);
        }
    }
}

// 이동 처리 (Move)
void AGimmickTestCharacter::Move(const FInputActionValue& Value)
{
    // 입력 값 (FVector2D)을 추출합니다.
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Y 축 (앞/뒤 이동) 처리
        if (MovementVector.Y != 0.f)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            AddMovementInput(Direction, MovementVector.Y);
        }

        // X 축 (좌/우 이동) 처리
        if (MovementVector.X != 0.f)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            AddMovementInput(Direction, MovementVector.X);
        }
    }
}

// 시점 회전 처리 (Look)
void AGimmickTestCharacter::Look(const FInputActionValue& Value)
{
    // 입력 값 (FVector2D)을 추출합니다.
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // X 축 (Yaw): 좌우 회전
        AddControllerYawInput(LookAxisVector.X);

        // Y 축 (Pitch): 상하 회전
        AddControllerPitchInput(LookAxisVector.Y);
    }
}
#pragma endregion

