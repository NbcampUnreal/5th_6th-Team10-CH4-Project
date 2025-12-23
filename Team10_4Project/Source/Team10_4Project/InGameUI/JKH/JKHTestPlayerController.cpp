#include "InGameUI/JKH/JKHTestPlayerController.h"
#include "InGameUI/JKH/VoiceChatSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"

AJKHTestPlayerController::AJKHTestPlayerController()
{
	PrimaryActorTick.bCanEverTick = false;
}
void AJKHTestPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] BeginPlay"));
	UGameInstance* GameInstance = GetGameInstance();

	if (IsValid(GameInstance))
	{
		UEOSSubsystem* EOSSubsystem = GameInstance->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			// 클라이언트
			if (!IsRunningDedicatedServer())
			{
				EOSSubsystem->LoginToEOS("localhost:8081", "test", "Developer");
			}
		}
	}
}

void AJKHTestPlayerController::FindSessions()
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] FindSessions"));
	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		UEOSSubsystem* EOSSubsystem = GameInstance->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			// 클라이언트
			EOSSubsystem->FindSessions();
		}
	}
}

void AJKHTestPlayerController::CreateLobbySession()
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerController] CreateLobbySession"));
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UEOSSubsystem* EOSSubsystem = GameInstance->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			EOSSubsystem->CreateLobbySession(4, false);
		}
	}
}