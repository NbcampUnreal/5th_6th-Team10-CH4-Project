# 네트워크 시스템 (Subsystem 패턴 - UE 5.6 권장)

## 1. SessionSubsystem.h - 세션 관리 서브시스템

```cpp
// SessionSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionSubsystem.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionsFound, const TArray<FString>&, SessionNames, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionJoined, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDestroyed, bool, bWasSuccessful);

USTRUCT(BlueprintType)
struct FSessionInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString SessionName;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentPlayers;

    UPROPERTY(BlueprintReadOnly)
    int32 MaxPlayers;

    UPROPERTY(BlueprintReadOnly)
    int32 Ping;

    FSessionInfo()
        : SessionName(""), CurrentPlayers(0), MaxPlayers(0), Ping(0)
    {}
};

UCLASS()
class SURVIVALGAME_API USessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Subsystem 생명주기
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 세션 생성
    UFUNCTION(BlueprintCallable, Category = "Session")
    void CreateSession(FName SessionName, int32 MaxPlayers, bool bIsLAN = false);

    // 세션 검색
    UFUNCTION(BlueprintCallable, Category = "Session")
    void FindSessions(bool bIsLAN = false);

    // 세션 참가
    UFUNCTION(BlueprintCallable, Category = "Session")
    void JoinSession(int32 SessionIndex);

    // 세션 종료
    UFUNCTION(BlueprintCallable, Category = "Session")
    void DestroySession();

    // 서버 트래블
    UFUNCTION(BlueprintCallable, Category = "Session")
    void ServerTravel(const FString& MapPath);

    // 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionCreated OnSessionCreatedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionsFound OnSessionsFoundDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionJoined OnSessionJoinedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Session")
    FOnSessionDestroyed OnSessionDestroyedDelegate;

    // 세션 정보 가져오기
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Session")
    TArray<FSessionInfo> GetFoundSessions() const { return FoundSessions; }

protected:
    // 온라인 서브시스템
    IOnlineSessionPtr SessionInterface;

    // 세션 설정
    TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

    // 세션 검색
    TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

    // 검색된 세션 정보
    UPROPERTY()
    TArray<FSessionInfo> FoundSessions;

    // 델리게이트 핸들
    FDelegateHandle OnCreateSessionCompleteDelegateHandle;
    FDelegateHandle OnFindSessionsCompleteDelegateHandle;
    FDelegateHandle OnJoinSessionCompleteDelegateHandle;
    FDelegateHandle OnDestroySessionCompleteDelegateHandle;

    // 콜백 함수
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
```

## 2. SessionSubsystem.cpp

```cpp
// SessionSubsystem.cpp
#include "SessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void USessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 온라인 서브시스템 가져오기
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();

        if (SessionInterface.IsValid())
        {
            // 델리게이트 바인딩
            OnCreateSessionCompleteDelegateHandle =
                SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
                    FOnCreateSessionCompleteDelegate::CreateUObject(
                        this, &USessionSubsystem::OnCreateSessionComplete));

            OnFindSessionsCompleteDelegateHandle =
                SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
                    FOnFindSessionsCompleteDelegate::CreateUObject(
                        this, &USessionSubsystem::OnFindSessionsComplete));

            OnJoinSessionCompleteDelegateHandle =
                SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
                    FOnJoinSessionCompleteDelegate::CreateUObject(
                        this, &USessionSubsystem::OnJoinSessionComplete));

            OnDestroySessionCompleteDelegateHandle =
                SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
                    FOnDestroySessionCompleteDelegate::CreateUObject(
                        this, &USessionSubsystem::OnDestroySessionComplete));

            UE_LOG(LogTemp, Log, TEXT("SessionSubsystem Initialized: %s"),
                *OnlineSubsystem->GetSubsystemName().ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Online Subsystem found!"));
    }
}

void USessionSubsystem::Deinitialize()
{
    // 델리게이트 정리
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
        SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
    }

    Super::Deinitialize();
}

void USessionSubsystem::CreateSession(FName SessionName, int32 MaxPlayers, bool bIsLAN)
{
    if (!SessionInterface.IsValid())
    {
        OnSessionCreatedDelegate.Broadcast(false);
        return;
    }

    // 기존 세션 제거
    auto ExistingSession = SessionInterface->GetNamedSession(SessionName);
    if (ExistingSession)
    {
        SessionInterface->DestroySession(SessionName);
    }

    // 세션 설정
    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    LastSessionSettings->bIsLANMatch = bIsLAN;
    LastSessionSettings->NumPublicConnections = MaxPlayers;
    LastSessionSettings->bShouldAdvertise = true;
    LastSessionSettings->bUsesPresence = true;
    LastSessionSettings->bUseLobbiesIfAvailable = true;
    LastSessionSettings->bAllowJoinInProgress = true;
    LastSessionSettings->bAllowJoinViaPresence = true;
    LastSessionSettings->bAllowInvites = true;

    // 커스텀 설정
    LastSessionSettings->Set(FName("SERVER_NAME"), FString("Survival Game Server"),
        EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    LastSessionSettings->Set(FName("MAP_NAME"), FString("GameMap"),
        EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    // 세션 생성
    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer)
    {
        bool bSuccess = SessionInterface->CreateSession(
            *LocalPlayer->GetPreferredUniqueNetId(), SessionName, *LastSessionSettings);

        if (!bSuccess)
        {
            OnSessionCreatedDelegate.Broadcast(false);
        }
    }
}

void USessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("Session Created: %s, Success: %d"),
        *SessionName.ToString(), bWasSuccessful);

    OnSessionCreatedDelegate.Broadcast(bWasSuccessful);

    if (bWasSuccessful)
    {
        // 게임 맵으로 서버 트래블
        ServerTravel("/Game/Maps/GameMap?listen");
    }
}

void USessionSubsystem::FindSessions(bool bIsLAN)
{
    if (!SessionInterface.IsValid())
    {
        OnSessionsFoundDelegate.Broadcast(TArray<FString>(), false);
        return;
    }

    LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
    LastSessionSearch->bIsLanQuery = bIsLAN;
    LastSessionSearch->MaxSearchResults = 50;
    LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer)
    {
        bool bSuccess = SessionInterface->FindSessions(
            *LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef());

        if (!bSuccess)
        {
            OnSessionsFoundDelegate.Broadcast(TArray<FString>(), false);
        }
    }
}

void USessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("Find Sessions Complete: Success: %d"), bWasSuccessful);

    TArray<FString> SessionNames;
    FoundSessions.Empty();

    if (bWasSuccessful && LastSessionSearch.IsValid())
    {
        for (const FOnlineSessionSearchResult& Result : LastSessionSearch->SearchResults)
        {
            FString ServerName;
            Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

            FString MapName;
            Result.Session.SessionSettings.Get(FName("MAP_NAME"), MapName);

            int32 CurrentPlayers = Result.Session.SessionSettings.NumPublicConnections -
                Result.Session.NumOpenPublicConnections;
            int32 MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;

            FSessionInfo Info;
            Info.SessionName = FString::Printf(TEXT("%s - %s (%d/%d)"),
                *ServerName, *MapName, CurrentPlayers, MaxPlayers);
            Info.CurrentPlayers = CurrentPlayers;
            Info.MaxPlayers = MaxPlayers;
            Info.Ping = Result.PingInMs;

            FoundSessions.Add(Info);
            SessionNames.Add(Info.SessionName);

            UE_LOG(LogTemp, Log, TEXT("Found Session: %s, Ping: %d"), *Info.SessionName, Info.Ping);
        }
    }

    OnSessionsFoundDelegate.Broadcast(SessionNames, bWasSuccessful);
}

void USessionSubsystem::JoinSession(int32 SessionIndex)
{
    if (!SessionInterface.IsValid() || !LastSessionSearch.IsValid())
    {
        OnSessionJoinedDelegate.Broadcast(false);
        return;
    }

    if (LastSessionSearch->SearchResults.Num() <= SessionIndex)
    {
        OnSessionJoinedDelegate.Broadcast(false);
        return;
    }

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (LocalPlayer)
    {
        bool bSuccess = SessionInterface->JoinSession(
            *LocalPlayer->GetPreferredUniqueNetId(),
            NAME_GameSession,
            LastSessionSearch->SearchResults[SessionIndex]);

        if (!bSuccess)
        {
            OnSessionJoinedDelegate.Broadcast(false);
        }
    }
}

void USessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    bool bSuccess = (Result == EOnJoinSessionCompleteResult::Success);

    UE_LOG(LogTemp, Log, TEXT("Join Session Complete: Success: %d"), bSuccess);

    OnSessionJoinedDelegate.Broadcast(bSuccess);

    if (bSuccess && SessionInterface.IsValid())
    {
        // 서버 주소 가져오기
        FString ConnectInfo;
        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
        {
            // 서버로 트래블
            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
                UE_LOG(LogTemp, Log, TEXT("Traveling to: %s"), *ConnectInfo);
            }
        }
    }
}

void USessionSubsystem::DestroySession()
{
    if (!SessionInterface.IsValid())
    {
        OnSessionDestroyedDelegate.Broadcast(false);
        return;
    }

    bool bSuccess = SessionInterface->DestroySession(NAME_GameSession);
    if (!bSuccess)
    {
        OnSessionDestroyedDelegate.Broadcast(false);
    }
}

void USessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("Session Destroyed: %s, Success: %d"),
        *SessionName.ToString(), bWasSuccessful);

    OnSessionDestroyedDelegate.Broadcast(bWasSuccessful);
}

void USessionSubsystem::ServerTravel(const FString& MapPath)
{
    UWorld* World = GetWorld();
    if (World && World->GetAuthGameMode())
    {
        World->ServerTravel(MapPath);
    }
}
```

## 3. ChatSubsystem.h - 채팅 서브시스템

```cpp
// ChatSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ChatSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChatMessageReceived, const FString&, PlayerName, const FString&, Message);

USTRUCT(BlueprintType)
struct FChatMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly)
    FString Message;

    UPROPERTY(BlueprintReadOnly)
    FDateTime Timestamp;

    FChatMessage()
        : PlayerName(""), Message(""), Timestamp(FDateTime::Now())
    {}
};

UCLASS()
class SURVIVALGAME_API UChatSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 채팅 메시지 전송 (클라이언트에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void SendChatMessage(const FString& Message);

    // 메시지 수신 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Chat")
    FOnChatMessageReceived OnChatMessageReceived;

    // 채팅 기록 가져오기
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Chat")
    TArray<FChatMessage> GetChatHistory() const { return ChatHistory; }

    // 채팅 기록 지우기
    UFUNCTION(BlueprintCallable, Category = "Chat")
    void ClearChatHistory() { ChatHistory.Empty(); }

protected:
    // 채팅 기록
    UPROPERTY()
    TArray<FChatMessage> ChatHistory;

    // 최대 메시지 수
    UPROPERTY(EditDefaultsOnly, Category = "Chat")
    int32 MaxChatHistory = 100;

    // 메시지 수신 처리 (서버/클라이언트 모두에서 호출)
    void ReceiveMessage(const FString& PlayerName, const FString& Message);

    friend class ASurvivalPlayerController;
};
```

## 4. VotingSubsystem.h - 투표 서브시스템

```cpp
// VotingSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VotingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVotingStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVotingEnded, APlayerController*, MostVotedPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVotesUpdated);

USTRUCT(BlueprintType)
struct FVoteData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly)
    int32 VoteCount;

    FVoteData() : PlayerName(""), VoteCount(0) {}
};

UCLASS()
class SURVIVALGAME_API UVotingSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 투표 시작 (서버 전용)
    UFUNCTION(BlueprintCallable, Category = "Voting")
    void StartVoting(float Duration);

    // 투표하기
    UFUNCTION(BlueprintCallable, Category = "Voting")
    void CastVote(APlayerController* Voter, APlayerController* Target);

    // 투표 종료 (서버 전용)
    UFUNCTION(BlueprintCallable, Category = "Voting")
    void EndVoting();

    // 투표 중인지 확인
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voting")
    bool IsVotingActive() const { return bIsVotingActive; }

    // 투표 결과 가져오기
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voting")
    TArray<FVoteData> GetVoteResults() const;

    // 최다 득표자
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voting")
    APlayerController* GetMostVotedPlayer() const;

    // 남은 시간
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voting")
    float GetRemainingTime() const { return RemainingTime; }

    // 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Voting")
    FOnVotingStarted OnVotingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Voting")
    FOnVotingEnded OnVotingEnded;

    UPROPERTY(BlueprintAssignable, Category = "Voting")
    FOnVotesUpdated OnVotesUpdated;

protected:
    // 투표 데이터
    UPROPERTY()
    TMap<APlayerController*, int32> Votes;

    // 투표 중 여부
    UPROPERTY()
    bool bIsVotingActive = false;

    // 남은 시간
    UPROPERTY()
    float RemainingTime = 0.0f;

    // 타이머
    FTimerHandle VoteTimerHandle;
};
```

## 5. 사용 예시 (Blueprint/C++)

### Blueprint에서 사용
```cpp
// 세션 생성
USessionSubsystem* SessionSubsystem = GetGameInstance()->GetSubsystem<USessionSubsystem>();
if (SessionSubsystem)
{
    SessionSubsystem->CreateSession(NAME_GameSession, 16, false);
}

// 세션 검색
SessionSubsystem->FindSessions(false);

// 채팅 메시지 전송
UChatSubsystem* ChatSubsystem = GetGameInstance()->GetSubsystem<UChatSubsystem>();
if (ChatSubsystem)
{
    ChatSubsystem->SendChatMessage("Hello!");
}

// 투표 시작
UVotingSubsystem* VotingSubsystem = GetGameInstance()->GetSubsystem<UVotingSubsystem>();
if (VotingSubsystem)
{
    VotingSubsystem->StartVoting(60.0f);
}
```

### Widget에서 델리게이트 바인딩
```cpp
// MainMenuWidget.cpp
void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    USessionSubsystem* SessionSubsystem = GetGameInstance()->GetSubsystem<USessionSubsystem>();
    if (SessionSubsystem)
    {
        SessionSubsystem->OnSessionCreatedDelegate.AddDynamic(this, &UMainMenuWidget::OnSessionCreated);
        SessionSubsystem->OnSessionsFoundDelegate.AddDynamic(this, &UMainMenuWidget::OnSessionsFound);
    }

    UChatSubsystem* ChatSubsystem = GetGameInstance()->GetSubsystem<UChatSubsystem>();
    if (ChatSubsystem)
    {
        ChatSubsystem->OnChatMessageReceived.AddDynamic(this, &UMainMenuWidget::OnChatMessage);
    }
}
```

## 장점 정리

### GameInstanceSubsystem 사용 시:
✅ 모듈화 - 기능별로 분리
✅ 재사용성 - 다른 프로젝트에서도 사용 가능
✅ 자동 생명주기 관리 - Initialize/Deinitialize 자동 호출
✅ Blueprint 친화적 - Get Subsystem 노드로 쉽게 접근
✅ 멀티플 서브시스템 - 여러 서브시스템 동시 사용 가능
✅ Epic 권장 패턴 - 최신 UE5 프로젝트 표준

이제 **서브시스템 패턴**을 사용하세요! 🎯
