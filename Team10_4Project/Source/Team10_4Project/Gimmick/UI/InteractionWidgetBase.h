// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TEAM10_4PROJECT_API UInteractionWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaciton")
	void UpdateText(const FText& InText);
};
