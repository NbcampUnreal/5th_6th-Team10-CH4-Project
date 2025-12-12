// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameUIWidget;
class APlayerController;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API AInGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    // (AInGameHUD를 상속받는) PlayerController가 생성될 때 호출
    virtual void BeginPlay() override;


    // 외부 (캐릭터/컨트롤러)에서 ASC를 받아와 UI를 초기화하는 함수.
    // BeginPlay 대신 이 함수가 Character::PossessedBy 등 안전한 시점에 호출
    // @param ASC 초기화에 사용할 Ability System Component
    void InitializeHUD(UAbilitySystemComponent* ASC);

protected:
    // HUD에 표시할 메인 위젯 클래스 (에디터/블루프린트에서 설정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UInGameUIWidget> MainUIWidgetClass;

private:
    // 실제 뷰포트에 표시될 위젯 인스턴스
    UPROPERTY()
    TObjectPtr<UInGameUIWidget> MainUIWidget;
};
