#include "InGameUI/JKH/VoteWidgetComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "GameState/Team10GameState.h"
#include "Character/CivilianPlayerState.h"
#include "Components/WidgetComponent.h"

UVoteWidgetComponent::UVoteWidgetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultBoxColor = FLinearColor::Black;
	DefaultBoxOpacity = 0.0f;

	HighlightedBoxColor = FLinearColor::Green;
	HighlightedBoxOpacity = 0.0f;
}

void UVoteWidgetComponent::NativeConstruct()
{
	Super::NativeConstruct();
	InitWidget();
}

void UVoteWidgetComponent::BindToWidget()
{
	if (!IsValid(OwnerPawn)) return;
	ACivilianPlayerState* PlayerState = OwnerPawn->GetPlayerState<ACivilianPlayerState>();

	if (IsValid(PlayerState))
	{
		if (!PlayerState->InitializeWidget.IsAlreadyBound(this, &ThisClass::InitWidget))
		{
			PlayerState->InitializeWidget.AddDynamic(this, &ThisClass::InitWidget);
		}
		if (!PlayerState->OnVoterListChanged.IsAlreadyBound(this, &ThisClass::UpdateWidget))
		{
			PlayerState->OnVoterListChanged.AddDynamic(this, &ThisClass::UpdateWidget);
		}
		if (!PlayerState->OnVoteTimerChanged.IsAlreadyBound(this, &ThisClass::UpdateTimer))
		{
			PlayerState->OnVoteTimerChanged.AddDynamic(this, &ThisClass::UpdateTimer);
		}
	}
}

void UVoteWidgetComponent::NativeDestruct()
{
	VoteBoxWidgets.Empty();
	VoteHorizontalBox->ClearChildren();

	if (IsValid(OwnerPawn))
	{
		if (ACivilianPlayerState* PlayerState = OwnerPawn->GetPlayerState<ACivilianPlayerState>())
		{
			if (PlayerState->InitializeWidget.IsAlreadyBound(this, &ThisClass::InitWidget))
			{
				PlayerState->InitializeWidget.RemoveDynamic(this, &ThisClass::InitWidget);
			}
			if (PlayerState->OnVoterListChanged.IsAlreadyBound(this, &ThisClass::UpdateWidget))
			{
				PlayerState->OnVoterListChanged.RemoveDynamic(this, &ThisClass::UpdateWidget);
			}
			if (PlayerState->OnVoteTimerChanged.IsAlreadyBound(this, &ThisClass::UpdateTimer))
			{
				PlayerState->OnVoteTimerChanged.RemoveDynamic(this, &ThisClass::UpdateTimer);
			}
		}
	}
	Super::NativeDestruct();
}

void UVoteWidgetComponent::InitWidget()
{
	if (!IsValid(VoteBoxWidgetClass)) return;
	
	VoteBoxWidgets.Empty();
	VoteHorizontalBox->ClearChildren();

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATeam10GameState* GameState = World->GetGameState<ATeam10GameState>();
	if (!IsValid(GameState)) return;
	int32 BorderCount = GameState->KillPlayerVotesCount;

	for (int32 i = 0; i < BorderCount; ++i)
	{
		UUserWidget* VoteBoxWidget = CreateWidget<UUserWidget>(this, VoteBoxWidgetClass);
		if (!IsValid(VoteBoxWidget)) return;

		if (i > 0)
		{
			USpacer* Spacer = NewObject<USpacer>(this, USpacer::StaticClass());
			Spacer->SetSize(FVector2D(10.0f, 50.0f));
			VoteHorizontalBox->AddChildToHorizontalBox(Spacer);
		}

		VoteHorizontalBox->AddChildToHorizontalBox(VoteBoxWidget);
		VoteBoxWidgets.Add(VoteBoxWidget);
	}
}

void UVoteWidgetComponent::UpdateWidget()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATeam10GameState* GameState = World->GetGameState<ATeam10GameState>();
	if (!IsValid(GameState)) return;

	if (!IsValid(OwnerPawn)) return;
	ACivilianPlayerState* PlayerState = OwnerPawn->GetPlayerState<ACivilianPlayerState>();
	if (!IsValid(PlayerState)) return;

	int32 HighlightSize = FMath::Min(PlayerState->VoterList.Num(), VoteBoxWidgets.Num());

	// 위젯 업데이트
	for (int32 i = 0; i < HighlightSize; ++i)
	{
		ApplyVoteHighlight(i);
	}
}

void UVoteWidgetComponent::ApplyVoteHighlight(int32 Index)
{
	UBorder* VoteBorder = Cast<UBorder>(VoteBoxWidgets[Index]->GetWidgetFromName(TEXT("VoteBorder")));
	if (!IsValid(VoteBorder)) return;

	VoteBorder->SetBrushColor(HighlightedBoxColor);
	VoteBorder->SetRenderOpacity(HighlightedBoxOpacity);
}

void UVoteWidgetComponent::UpdateTimer(int32 NewTime)
{
	if (!VoteTimerText) return;
	FText TextTime = FText::FromString(FString::FromInt(NewTime));
	VoteTimerText->SetText(TextTime);
}
