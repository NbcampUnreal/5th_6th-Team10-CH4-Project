#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VoteWidgetComponent.generated.h"

class UHorizontalBox;
class UTextBlock;

UCLASS()
class TEAM10_4PROJECT_API UVoteWidgetComponent : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UVoteWidgetComponent(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindToWidget();

	UFUNCTION()
	void InitWidget();
	void RefreshWidget();
	UFUNCTION()
	void UpdateWidget();
	void ApplyVoteHighlight(int32 Index);
	UFUNCTION()
	void UpdateTimer(int32 NewTime);
	void SetOwnerPawn(APawn* InPawn) { OwnerPawn = InPawn; };
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Owner")
	TObjectPtr<APawn> OwnerPawn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> VoteHorizontalBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VoteTimerText;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> VoteBoxWidgetClass;
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> VoteBoxWidgets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	FLinearColor DefaultBoxColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	float DefaultBoxOpacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	FLinearColor HighlightedBoxColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	float HighlightedBoxOpacity;
};
