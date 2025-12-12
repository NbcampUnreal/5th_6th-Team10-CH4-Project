#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

struct FChatMessage;
class UEditableTextBox;
class UVerticalBox;

UCLASS()
class TEAM10_4PROJECT_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInput; // ChatInput
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ChatMessageList; // ChatMessageList
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	void ChatInputReady();
	UFUNCTION()
	void OnChatMessage(const FChatMessage& ChatMessage);
};
