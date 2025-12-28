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
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
	
	bool InitWidget();
	void RefreshWidget();
	void OnUpdateWidget();
	void ApplyVoteHighlight(int32 Index);
	void OnVoteReceived();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> VoteHorizontalBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VoteTimerText;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> VoteBoxWidgetClass;
	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> VoteBoxWidgets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	FLinearColor DefaultVoteBoxColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	float DefaultVoteBoxOpacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	FLinearColor ActivateVoteBoxColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets|Design")
	float ActivateVoteBoxOpacity;
};
