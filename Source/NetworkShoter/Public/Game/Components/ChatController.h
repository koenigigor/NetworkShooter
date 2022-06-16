// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NSStructures.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "ChatController.generated.h"

class ANSPlayerState;
struct FDamageInfo;

//todo not DYNAMIC_MULTICAST
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMessageReceive, ANSPlayerState*, FromPlayer, FString, Message);

/**
 * send and receive chat messages
 * adds to GameState class
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UChatController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChatController();
	virtual void BeginPlay() override;

	/** [Server] Client send message in chat */
	void SendMessage_Player(const FString& Message, ANSPlayerState* FromWho);

	/** [Multicast] Send message to clients */
	UFUNCTION(NetMulticast, Unreliable)
	void ReceiveMessage(const FString& Message, ANSPlayerState* FromWho);

	UPROPERTY()
	FMessageReceive MessageReceive;

protected:
	/** Send damage info in chat */
	void OnReceiveDamage(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	/** [Multicast] Send message to clients */
	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastDamageInfo(FDamageInfo DamageInfo);
	
	FGameplayMessageListenerHandle ListenerHandle;
};
