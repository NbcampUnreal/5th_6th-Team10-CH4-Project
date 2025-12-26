#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionSubsystem.generated.h"

/**
 * OutGame 전용 세션 관리자
 * - 서버 생성
 * - 서버 검색
 * - 서버 참가
 */
UCLASS()
class TEAM10_4PROJECT_API USessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // === 세션 생성 ===
    void CreateSession(bool bIsDedicatedServer);

    // === 세션 검색 ===
    void FindSessions();

    // === 세션 참가 ===
    void JoinSession(const FOnlineSessionSearchResult& SearchResult);

private:
    /** Online Session Interface */
    IOnlineSessionPtr SessionInterface;

    /** 검색 결과 저장 */
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

private:
    // Delegate 콜백
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
