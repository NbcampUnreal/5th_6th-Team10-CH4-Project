// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI/KSH/InGameHUD.h"
#include "InGameUI/KSH/InGameUIWidget.h"
#include "AbilitySystemComponent.h"
#include "Team10_4ProjectPlayerController.h"

void AInGameHUD::BeginPlay()
{
    Super::BeginPlay();

    // 위젯 클래스가 에디터에서 설정되었는지 확인
    if (!MainUIWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("HUD: !MainUIWidgetClass"));
        return;
    }

    // 위젯 생성 (MainUIWidget 변수 유효성 확인)
    if (!MainUIWidget)
    {
        APlayerController* PC = GetOwningPlayerController();
        if (PC)
        {
            MainUIWidget = CreateWidget<UInGameUIWidget>(PC, MainUIWidgetClass);
        }
    }

    // 위젯을 뷰포트에 추가
    if (MainUIWidget && !MainUIWidget->IsInViewport())
    {
        MainUIWidget->AddToViewport();
        UE_LOG(LogTemp, Log, TEXT("HUD: MainUIWidget created"));
    }
}

void AInGameHUD::InitializeHUD(UAbilitySystemComponent* ASC)
{
    // BeginPlay에서 위젯이 생성된 경우
    if (!MainUIWidget || !ASC)
    {
        UE_LOG(LogTemp, Error, TEXT("HUD: !MainUIWidget or !ASC"));
        return;
    }

    // UInGameUIWidget의 초기화 로직 호출 (GAS 연결)
    MainUIWidget->InitializeUI(ASC);
    UE_LOG(LogTemp, Log, TEXT("HUD: InGameUI initialized with ASC."));
}
