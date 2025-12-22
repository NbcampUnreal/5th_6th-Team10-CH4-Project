#include "SessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/Engine.h"

#include "OnlineSessionSettings.h"

void USessionSubsystem::CreateSession(bool bIsDedicatedServer)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem not found"));
        return;
    }

    SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
        return;

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsDedicated = bIsDedicatedServer;
    SessionSettings.bIsLANMatch = true;               // ÀÏ´Ü LAN
    SessionSettings.NumPublicConnections = 4;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinInProgress = true;

    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
        this,
        &USessionSubsystem::OnCreateSessionComplete
    );

    SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

void USessionSubsystem::FindSessions()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
        return;

    SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
        return;

    SessionSearch = MakeShared<FOnlineSessionSearch>();
    SessionSearch->MaxSearchResults = 100;
    SessionSearch->bIsLanQuery = false;

    SessionSearch->QuerySettings.Set(
        FName("PRESENCE"),
        true,
        EOnlineComparisonOp::Equals
    );

    SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
        this,
        &USessionSubsystem::OnFindSessionsComplete
    );

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void USessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
    if (!SessionInterface.IsValid())
        return;

    SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
        this,
        &USessionSubsystem::OnJoinSessionComplete
    );

    SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
}

void USessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("CreateSession Complete: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Fail"));

    if (bWasSuccessful)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            World->ServerTravel(TEXT("/Game/Maps/GameplayMap?listen"));
        }
    }
}

void USessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("FindSessions Complete"));

    if (!bWasSuccessful || !SessionSearch.IsValid())
        return;

    UE_LOG(LogTemp, Log, TEXT("Found %d Sessions"), SessionSearch->SearchResults.Num());
}

void USessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Log, TEXT("JoinSession Complete"));

    FString Address;
    if (SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
        }
    }
}
