// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NSStructures.h"
#include "Chat/ChatDefaultChannelsInfo.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "ChatController.generated.h"

class UChatStyleSave;
struct FDamageInfo;


USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_BODY()

	/** Channel tag */
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadWrite)
	APlayerState* From = nullptr;

	/** ServerTimeSeconds when message send */
	UPROPERTY(BlueprintReadWrite)
	float Time = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FString Message;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceiveDelegate, const FChatMessage&, Message);

/** Proxy for send message from local clients
 *	Must be attached to player state */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UChatProxy : public UActorComponent
{
	GENERATED_BODY()
public:
	UChatProxy();

	/** Proxy function for send chat message from chat widget to server */
	UFUNCTION(Server, Unreliable)
	void SendChatMessage(const FChatMessage& Message);

	/** Proxy function for receive message specified only for this client (private, location, etc.) */
	UFUNCTION(Client, Unreliable)
	void ReceiveChatMessage(const FChatMessage& Message);
};


/** Send and receive chat messages,
 *  Must be attached to GameState */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UChatController : public UActorComponent
{
	GENERATED_BODY()

public:
	UChatController();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, DisplayName = "GetChatController", meta = (DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"))
	static UChatController* Get(UObject* WorldContextObject);

    UFUNCTION(BlueprintPure)
    UChatStyleSave* GetChatStyle() const { return ChatStyle; }

	
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendMessage(const FChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendMessage_Local(const FChatMessage& Message);

	UPROPERTY(BlueprintAssignable, Transient)
	FOnMessageReceiveDelegate OnReceiveMessage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UChatDefaultChannelsInfo* DefaultChannelsData = nullptr;
	

protected:
	/** If Message should be received only few players (Location, Group) */
	void GetRecipientsForMessage(const FChatMessage& Message, TArray<UChatProxy*>& OutRecipients);
	
	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastMessage(FChatMessage Message);

	/** Receive DamageInfo, and send it in chat */
	void OnReceiveDamage(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	UPROPERTY()
	UChatStyleSave* ChatStyle = nullptr;
	
	FGameplayMessageListenerHandle DamageInfoListenerHandle;
};

//todo found IOnlineChat and OnlineMessageInterface in online subsystem, learn and maybe use
