#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ChatSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMessageType : uint8
{
   None,
   Normal,
   Team,
   System,
   End
};

USTRUCT(BlueprintType)
struct FChatMessage
{
   GENERATED_BODY()

   UPROPERTY(BlueprintReadOnly)
   EMessageType MessageType;

   UPROPERTY(BlueprintReadOnly)
   FString PlayerName;

   UPROPERTY(BlueprintReadOnly)
   FString Message;

   UPROPERTY(BlueprintReadOnly)
   FDateTime Timestamp;

   FChatMessage(EMessageType MessageType = EMessageType::None,
      FString PlayerName = TEXT(""),
      FString Message = TEXT(""),
      FDateTime Timestamp = FDateTime::Now()
      )
   {
      this->MessageType = MessageType;
      this->PlayerName = PlayerName;
      this->Message = Message;
      this->Timestamp = Timestamp;
   }
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageReceived, const FChatMessage&, ChatMessage);

UCLASS()
class TEAM10_4PROJECT_API UChatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
   // 메시지 수신 델리게이트
   UPROPERTY(BlueprintAssignable, Category = "Chat")
   FOnChatMessageReceived OnChatMessageReceived;

   UChatSubsystem();

   // 채팅 메시지 전송 (클라이언트에서 호출)
   UFUNCTION(BlueprintCallable, Category = "Chat")
   void SendChatMessage(const FChatMessage& ChatMessage);

   // 메시지 수신 처리 (서버/클라이언트 모두에서 호출)
   void ReceiveChatMessage(const FChatMessage& ChatMessage);

   // 채팅 기록 가져오기
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Chat")
   TArray<FChatMessage> GetChatHistory() const { return ChatHistory; }

   // 채팅 기록 지우기
   UFUNCTION(BlueprintCallable, Category = "Chat")
   void ClearChatHistory();

   int32 GetMaxChatHistory() const { return MaxChatHistory; };
   // UFUNCTION(Server, Reliable)
   // void APlayerController::ServerRPC_SendChatMessage(const FChatMessage& ChatMessage);
protected:
   // 채팅 기록
   UPROPERTY()
   TArray<FChatMessage> ChatHistory;

   // 최대 메시지 수
   UPROPERTY(EditDefaultsOnly, Category = "Chat")
   int32 MaxChatHistory = 100;

   //friend class ASurvivalPlayerController;
};
