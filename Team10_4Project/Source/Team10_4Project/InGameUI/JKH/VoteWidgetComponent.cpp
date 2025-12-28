#include "InGameUI/JKH/VoteWidgetComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/Border.h"
#include "GameState/Team10GameState.h"
#include "Character/CivilianPlayerState.h"

UVoteWidgetComponent::UVoteWidgetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (IsValid(VoteBoxWidgetClass))
	{
		UUserWidget* VoteBoxWidget = CreateWidget<UUserWidget>(this, VoteBoxWidgetClass);
		UBorder* VoteBorder = Cast<UBorder>(VoteBoxWidget->GetWidgetFromName(TEXT("VoteBorder")));
		if (!IsValid(VoteBorder)) return;
		
		DefaultVoteBoxColor = VoteBorder->GetBrushColor();
		DefaultVoteBoxOpacity = VoteBorder->GetRenderOpacity();
	}
	else
	{
		DefaultVoteBoxColor = FLinearColor::Black;
		DefaultVoteBoxOpacity = 100.0f;
	}
}

bool UVoteWidgetComponent::Initialize()
{
	bool bReturn = Super::Initialize();

	if (!InitWidget()) return bReturn;

	ACivilianPlayerState* PlayerState = GetOwningPlayerState<ACivilianPlayerState>();
	if (IsValid(PlayerState))
	{
		if (!PlayerState->OnVoterListChanged.IsAlreadyBound(this, &ThisClass::OnUpdateWidget))
		{
			PlayerState->OnVoterListChanged.AddDynamic(this, &ThisClass::OnUpdateWidget);
		}
	}

	return bReturn;
}

void UVoteWidgetComponent::NativeDestruct()
{
	VoteBoxWidgets.Empty();

	ACivilianPlayerState* PlayerState = GetOwningPlayerState<ACivilianPlayerState>();
	if (IsValid(PlayerState))
	{
		if (PlayerState->OnVoterListChanged.IsAlreadyBound(this, &ThisClass::OnUpdateWidget))
		{
			PlayerState->OnVoterListChanged.RemoveDynamic(this, &ThisClass::OnUpdateWidget);
		}
	}

	Super::NativeDestruct();
}

bool UVoteWidgetComponent::InitWidget()
{
	if (!IsValid(VoteBoxWidgetClass)) return false;

	UWorld* World = GetWorld();
	if (!IsValid(World)) return false;

	ATeam10GameState* GameState = World->GetGameState<ATeam10GameState>();
	if (!IsValid(GameState)) return false;
	int32 BorderCount = GameState->KillPlayerVotesCount;

	for (int32 i = 0; i < BorderCount; ++i)
	{
		UUserWidget* VoteBoxWidget = CreateWidget<UUserWidget>(this, VoteBoxWidgetClass);
		if (!IsValid(VoteBoxWidget)) return false;

		if (i > 0)
		{
			USpacer* Spacer = NewObject<USpacer>(this, USpacer::StaticClass());
			Spacer->SetSize(FVector2D(10.0f, 50.0f));
			VoteHorizontalBox->AddChildToHorizontalBox(Spacer);
		}

		VoteHorizontalBox->AddChildToHorizontalBox(VoteBoxWidget);
		VoteBoxWidgets.Add(VoteBoxWidget);
	}

	return true;
}

void UVoteWidgetComponent::RefreshWidget()
{
	if (!InitWidget()) return;		// 초기화 실패

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATeam10GameState* GameState = World->GetGameState<ATeam10GameState>();
	if (!IsValid(GameState)) return;

	APlayerState* PlayerState = GetOwningPlayerState();
	if (!PlayerState) return;
}

void UVoteWidgetComponent::OnUpdateWidget()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATeam10GameState* GameState = World->GetGameState<ATeam10GameState>();
	if (!IsValid(GameState)) return;

	ACivilianPlayerState* PlayerState = GetOwningPlayerState<ACivilianPlayerState>();
	if (!IsValid(PlayerState)) return;

	TArray<TObjectPtr<APlayerState>> VotersList = PlayerState->GetVoterList();
	
	if (VotersList.Num() != VoteBoxWidgets.Num())	// 플레이어 수와 투명 가능 수가 다르면
	{
		InitWidget();
	}

	// 위젯 업데이트
	for (int32 i = 0; i < VoteBoxWidgets.Num(); ++i)
	{
		ApplyVoteHighlight(i);
	}
}

void UVoteWidgetComponent::ApplyVoteHighlight(int32 Index)
{
	if (Index > 0)
	{
		// HorizontalBox에 간격을 위해 삽입한 Spacer들을 고려
		Index *= 2;
	}

	UBorder* VoteBorder = Cast<UBorder>(VoteBoxWidgets[Index]->GetWidgetFromName(TEXT("VoteBorder")));
	if (!IsValid(VoteBorder)) return;

	VoteBorder->SetBrushColor(FLinearColor::Green);
}

void UVoteWidgetComponent::OnVoteReceived()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	ATeam10GameState* GameState = World->GetGameState<ATeam10GameState>();
	if (!IsValid(GameState)) return;

	ACivilianPlayerState* PlayerState = GetOwningPlayerState<ACivilianPlayerState>();
	if (!IsValid(PlayerState)) return;

	TArray<TObjectPtr<APlayerState>> VotersList = PlayerState->GetVoterList();
	ApplyVoteHighlight(VotersList.Num() - 1);
}


