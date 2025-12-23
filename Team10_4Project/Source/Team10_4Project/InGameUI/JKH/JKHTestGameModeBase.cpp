#include "InGameUI/JKH/JKHTestGameModeBase.h"
#include "InGameUI/JKH/EOSSubsystem.h"

AJKHTestGameModeBase::AJKHTestGameModeBase()
{
	GameSessionClass = UEOSSubsystem::StaticClass();
}

void AJKHTestGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[GameMode] BeginPlay"));

	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		UEOSSubsystem* EOSSubsystem = GameInstance->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			// 데디케이티드 서버 체크
			if (IsRunningDedicatedServer())
			{
				UE_LOG(LogTemp, Log, TEXT("[GameMode] DedicatedServer Detected."));
				// EOSSubsystem->CreateGameSession(4, false);
				// EOSSubsystem->LoginToEOS("localhost:8081", "test", "Developer");

			}
			else
			{
				// 리슨 서버 ?
				// EOSSubsystem->LoginToEOS("localhost:8081", "test", "Developer");

			}
		}
	}
}