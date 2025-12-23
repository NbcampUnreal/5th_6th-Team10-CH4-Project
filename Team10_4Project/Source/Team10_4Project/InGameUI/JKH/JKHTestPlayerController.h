#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "JKHTestPlayerController.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AJKHTestPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AJKHTestPlayerController();
	UFUNCTION(Exec)
	void FindSessions();
	UFUNCTION(Exec)
	void CreateLobbySession();

protected:
	virtual void BeginPlay() override;
};
