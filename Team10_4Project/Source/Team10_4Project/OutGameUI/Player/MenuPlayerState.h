// MenuPlayerState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MenuPlayerState.generated.h"

UCLASS()
class TEAM10_4PROJECT_API AMenuPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AMenuPlayerState();

    FORCEINLINE bool IsReady() const { return bIsReady; }

    void SetReady(bool bNewReady);

    UFUNCTION()
    void OnRep_Ready();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_Ready, BlueprintReadOnly, Category = "Lobby")
    bool bIsReady = false;
};