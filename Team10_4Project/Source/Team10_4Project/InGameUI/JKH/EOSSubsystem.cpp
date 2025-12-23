#include "EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/VoiceInterface.h"

UEOSSubsystem::UEOSSubsystem()
{
}

void UEOSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UGameInstance* GI = GetGameInstance();
	FWorldContext* WorldContext = GI->GetWorldContext();
	int32 PIEIndex = WorldContext->PIEInstance;
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] PIEInstance: %d"), PIEIndex);

	// 온라인 서브시스템 가져오기
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		IdentityInterface = Subsystem->GetIdentityInterface();
		VoiceInterface = Subsystem->GetVoiceInterface();

		if (SessionInterface.IsValid() && IdentityInterface.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Initialized successfully."));
		}
	}
	else
	{
		// EOS 로드 실패
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to get EOS Subsystem! Check Plugins/Artifact settings."));
	}

	if (VoiceInterface.IsValid())
	{
		// 누가 말하는지 감지하는 델리게이트 등록
		PlayerTalkingDelegateHandle = VoiceInterface->AddOnPlayerTalkingStateChangedDelegate_Handle(
			FOnPlayerTalkingStateChangedDelegate::CreateUObject(this, &UEOSSubsystem::OnPlayerTalkingStateChanged));
	}
}

void UEOSSubsystem::Deinitialize()
{
	// 델리게이트 해제
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(LoginCompleteDelegateHandle);
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateGameSessionCompleteDelegateHandle);
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateLobbySessionCompleteDelegateHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}
	if (IdentityInterface.IsValid())
	{
		IdentityInterface->ClearOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegateHandle);
	}

	Super::Deinitialize();
}

void UEOSSubsystem::LoginToEOS(FString ID, FString Token, FString LoginType)
{
	if (!IdentityInterface.IsValid()) return;

	FOnlineAccountCredentials Credentials;
	Credentials.Id = ID;					// 예: DevAuthTool의 주소 (localhost:8081)
	Credentials.Token = Token;			// 예: DevAuthTool의 Credential Name (User1)
	Credentials.Type = LoginType;		// 예: "Developer" 혹은 "AccountPortal"

	UGameInstance* GI = GetGameInstance();
	FWorldContext* WorldContext = GI->GetWorldContext();
	int32 PIEIndex = WorldContext->PIEInstance;

	if (PIEIndex == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] UserClient try loggin"));
		Credentials.Token = TEXT("test2"); // 미리 만들어둔 Credential Name
	}

	LoginCompleteDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(
		0, FOnLoginCompleteDelegate::CreateUObject(this, &UEOSSubsystem::OnLoginComplete));

	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem]Logging into EOS..."));
	bool bWasSuccessful = IdentityInterface->Login(0, Credentials);
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to start EOS Login request."));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(LoginCompleteDelegateHandle);
	}
}

void UEOSSubsystem::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(LoginCompleteDelegateHandle);

	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] OnLoginComplete: %s. Error: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"), *Error);

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Login success."));
		//if (!IsRunningDedicatedServer()) return;
		// CreateLobbySession(4, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Login failed."));
		if (!IsRunningDedicatedServer()) return;
	}
}

void UEOSSubsystem::CreateGameSession(int32 MaxNumPlayers, bool bIsLAN)
{
	if (!SessionInterface.IsValid()) return;
	if (!IsRunningDedicatedServer()) return;

	CreateGameSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateGameSessionComplete));

	// 세션 설정 구성
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = bIsLAN;
	SessionSettings.NumPublicConnections = MaxNumPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = false;		// 데디케이트 서버는 비활성화
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	// **중요**: EOS 로비를 사용하려면 이 값이 true여야 합니다.

	// [보이스챗 설정]
	SessionSettings.Set(FName("EnableRTC"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(SETTING_MAPNAME, FString("LobbyMap"), EOnlineDataAdvertisementType::ViaOnlineService);
	// 세션 생성 요청
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Creating EOS Session..."));
	
	bool bWasSuccessful = SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
	
	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to start session creation request."));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateGameSessionCompleteDelegateHandle);
	}
}

void UEOSSubsystem::OnCreateGameSessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateGameSessionCompleteDelegateHandle);
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] OnCreateSessionComplete: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Session created successfully automatically!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to create session."));
	}
}

void UEOSSubsystem::CreateLobbySession(int32 MaxPlayers, bool bIsLan)
{
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Creation LobbySession ..."));
	if (IsRunningDedicatedServer()) 
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Dedicated servers cannot create lobbies."));
		return;
	}
	if (!SessionInterface.IsValid()) return;

	CreateLobbySessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateLobbySessionComplete));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = bIsLan;
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true; // 초대를 위해 필수
	SessionSettings.bShouldAdvertise = true;      // 검색 노출
	SessionSettings.bUsesPresence = true;			 // 로비
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;

	// [보이스챗 설정]
	//SessionSettings.Set(FName("EnableRTC"), true, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(SEARCH_KEYWORDS, FString("MyGameLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

	FUniqueNetIdPtr NetId = IdentityInterface->GetUniquePlayerId(0);
	if (!NetId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Invalid UniqueNetId. Player might not be logged in."));
		return;
	}

	// 중복된 NAME_GameSession 세션 파괴
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	// 로비 세션 생성
	bool bWasSuccessful = SessionInterface->CreateSession(*NetId, NAME_GameSession, SessionSettings);
	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to start lobby creation request."));
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateLobbySessionCompleteDelegateHandle);
	}
}

void UEOSSubsystem::OnCreateLobbySessionComplete(FName SessionName, bool bWasSuccessful)
{
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateLobbySessionCompleteDelegateHandle);
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] OnCreateLobbyComplete: %s"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"));

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Lobby created successfully automatically!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to create lobby."));
	}
}

void UEOSSubsystem::FindSessions()
{
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 50;        // 검색할 최대 방 개수
	SessionSearch->PingBucketSize = 50;          // 핑 설정

	// Lobby
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	// Session
	// SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, false, EOnlineComparisonOp::Equals);
	// [선택] 특정 키워드로 방을 걸러내고 싶다면 (생성 시 SEARCH_KEYWORDS를 설정했다면)
	SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("MyGameLobby"), EOnlineComparisonOp::Equals);

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete));

	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Starting session search..."));

	bool bWasSuccessful = SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to start session search request."));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
}


void UEOSSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Session search completed successfully."));

		int32 SearchResultCount = SessionSearch->SearchResults.Num();
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Found %d sessions."), SearchResultCount);

		// 결과 순회 (디버그용 로그)
		if (SearchResultCount > 0)
		{
			for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
			{
				UE_LOG(LogTemp, Log, TEXT(" - Session ID: %s"), *Result.GetSessionIdStr());
				UE_LOG(LogTemp, Log, TEXT(" - Owner: %s"), *Result.Session.OwningUserName);
				UE_LOG(LogTemp, Log, TEXT(" - Ping: %d ms"), Result.PingInMs);

				// 여기서 찾은 방에 바로 참가하려면 JoinSession을 호출하면 됩니다.
				// 보통은 UI 리스트에 이 Result 정보를 뿌려줍니다.
			}
			GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
				JoinSession(SessionSearch->SearchResults[0]);
				});
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Session search failed."));
	}
}

void UEOSSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!SessionInterface.IsValid()) return;

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete));

	// LocalUserNum: 보통 0 (로컬 플레이어)
	// SessionName: 클라이언트 내부에서 관리할 이름 (보통 NAME_GameSession 사용)
	// SearchResult: FindSessions에서 찾은 결과 중 선택한 것
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Requesting join session..."));

	bool bWasSuccessful = SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to start join session request."));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
}

void UEOSSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	if (!SessionInterface.IsValid()) return;

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Join session successful."));
		// UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Preparing to travel..."));
		// 접속 주소(Connect String) 가져오기
		FString ConnectString;
		if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString))
		{
			// Travel
			/*
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				// UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Traveling to: %s"), *ConnectString);
				// PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
			}
			*/
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Failed to resolve connect string!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSubsystem] Join session failed. Result Code: %d"), (int32)Result);
	}
}

void UEOSSubsystem::StartSpeaking()
{
	if (!VoiceInterface.IsValid()) return;
	// 로컬 유저(0번)의 마이크 입력 전송 시작
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Start Speaking..."));
	VoiceInterface->StartNetworkedVoice(0);
}
void UEOSSubsystem::StopSpeaking()
{
	if (!VoiceInterface.IsValid()) return;
	// 로컬 유저(0번)의 마이크 입력 전송 중단
	UE_LOG(LogTemp, Log, TEXT("[EOSSubsystem] Stop Speaking."));
	VoiceInterface->StopNetworkedVoice(0);
}

void UEOSSubsystem::OnPlayerTalkingStateChanged(TSharedRef<const FUniqueNetId> PlayerId, bool bIsTalking)
{
	if (bIsTalking)
	{
		UE_LOG(LogTemp, Log, TEXT("[Voice] Player %s began talking."), *PlayerId->ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Voice] Player %s stopped talking."), *PlayerId->ToString());
	}
}
