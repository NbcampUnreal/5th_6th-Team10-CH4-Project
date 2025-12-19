// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUIWidget.generated.h"

// 전방 선언
class UHealthBarWidget;
class UStaminaBarWidget;
class UAbilitySystemComponent;
class UInventoryWidget;

/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UInGameUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 위젯 바인딩 (Bind Widget) UMG Designer에서 위젯을 C++ 변수와 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHealthBarWidget> HealthBar; // 체력바 위젯
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStaminaBarWidget> StaminaBar; // 스태미나바 위젯
    //UPROPERTY(meta = (BindWidget))
    //TObjectPtr<UInventoryWidget> Inventory; // 인벤토리 위젯 바인딩


    // 기타 UI 요소들 (예: 인벤토리 등) 추가

    // ASC를 받아 하위 위젯들을 초기화하는 핵심 함수
    void InitializeUI(UAbilitySystemComponent* ASC);

protected:
    // 위젯이 생성될 때 호출됩니다.
    virtual bool Initialize() override;
	
};
