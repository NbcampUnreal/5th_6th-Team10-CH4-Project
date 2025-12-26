// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
// 리플렉션 시스템을 위한 UClass 정의
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

// 실제 C++ 인터페이스
class TEAM10_4PROJECT_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 상호작용 로직: 서버에서 실행할 핵심 함수 (BlueprintNativeEvent 사용)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gimmick|Interaction")
	void Interact(AActor* Instigator);	// Instigator: 상호작용을 시도한 플레이어

	// 상호작용 텍스트: UI 표시용
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gimmick|Interaction")
	FText GetInteractText(AActor* _Instigator) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gimmick|Interaction")
	void SetInteractionUI(bool bVisible, AActor* _Instigator);
};
