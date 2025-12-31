#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JKHTestGameModeBase.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AJKHTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AJKHTestGameModeBase();
	virtual void BeginPlay() override;
};
