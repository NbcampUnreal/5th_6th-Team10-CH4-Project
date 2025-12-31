// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h" 
#include "AbilitySystemComponent.h"

UAnimNotify_SendGameplayEvent::UAnimNotify_SendGameplayEvent()
{
}

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;
	
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	// 이벤트 전송
	FGameplayEventData Payload;
	Payload.Instigator = OwnerActor;
	Payload.Target = OwnerActor;
	
	UE_LOG(LogTemp, Warning, TEXT("[AnimNotify] Activate SendGamePlayEvent.!!!"));
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
}
