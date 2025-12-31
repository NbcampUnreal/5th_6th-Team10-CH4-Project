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

    bool IsReady() const { return bIsReady; }
    void SetReady(bool bNewReady);

    UFUNCTION()
    void OnRep_Ready();

    // override 키워드가 정확히 붙어 있는지 확인
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    // ReplicatedUsing이 정확한지 확인
    UPROPERTY(ReplicatedUsing = OnRep_Ready, BlueprintReadOnly, Category = "Lobby")
    bool bIsReady = false;
};