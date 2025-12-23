#pragma once
#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSSubsystem.generated.h"

UCLASS()
class TEAM10_4PROJECT_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UEOSSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 로그인 =============================================
public:
	void LoginToEOS(FString ID, FString Token, FString LoginType);
private:
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	FDelegateHandle LoginCompleteDelegateHandle;

	// 세션 ===============================================
public:
	void CreateGameSession(int32 MaxNumPlayers, bool bIsLAN);
private:
	void OnCreateGameSessionComplete(FName SessionName, bool bWasSuccessful);
	FDelegateHandle CreateGameSessionCompleteDelegateHandle;
	IOnlineSessionPtr SessionInterface;
	IOnlineIdentityPtr IdentityInterface;

	// 로비 ===============================================
public:
	void CreateLobbySession(int32 MaxPlayers, bool bIsLan);
private:
	void OnCreateLobbySessionComplete(FName SessionName, bool bWasSuccessful);
	FDelegateHandle CreateLobbySessionCompleteDelegateHandle;

	// 세션 찾기 ==========================================
public:
	void FindSessions();
	TSharedPtr<FOnlineSessionSearch> GetSessionSearch() const { return SessionSearch; };
private:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	void OnFindSessionsComplete(bool bWasSuccessful);
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	// 세션 참가 ==========================================
public:
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);
private:
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	FDelegateHandle JoinSessionCompleteDelegateHandle;
};
