// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatController.h"

#include "NetworkShoter.h"
#include "Area/AreaVolume.h"
#include "Chat/ChatStyleSave.h"
#include "Game/NSPlayerState.h"
#include "Game/PCNetShooter.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"


DEFINE_LOG_CATEGORY_STATIC(LogChatController, All, All);


UChatProxy::UChatProxy()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UChatProxy::SendChatMessage_Implementation(const FChatMessage& Message)
{
	if (const auto ChatController = UChatController::Get(this))
	{
		ChatController->SendMessage(Message);
	}
}

void UChatProxy::ReceiveChatMessage_Implementation(const FChatMessage& Message)
{
	if (const auto ChatController = UChatController::Get(this))
	{
		ChatController->SendMessage_Local(Message);
	}	
}


/* ------------ */


UChatController::UChatController()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

UChatController* UChatController::Get(UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld() && WorldContextObject->GetWorld()->GetGameState())
	{
		const auto Chat = WorldContextObject->GetWorld()->GetGameState()->FindComponentByClass<UChatController>();
		UE_CLOG(!Chat, LogChatController, Warning, TEXT("Chat controller must be game state component, ensure it pug into GameState"));
		return Chat;
	}

	return nullptr;
}

void UChatController::SendMessage(const FChatMessage& Message)
{
	//client redirect message to server and return 
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		if (const auto ChatProxy = GetWorld()->GetFirstPlayerController()->FindComponentByClass<UChatProxy>())
		{
			ChatProxy->SendChatMessage(Message);
		}
		else UE_LOG(LogChatController, Warning, TEXT("Can't sent message from client, UChatProxy componemt not found in player controller"));
		return;
	}


	//check if message location / private etc. sent only for this clients
	TArray<UChatProxy*> MessageRecipients;
	GetRecipientsForMessage(Message, MessageRecipients);
	if (MessageRecipients.Num() > 0)
	{
		for (const auto& Recipient : MessageRecipients)
		{
			Recipient->ReceiveChatMessage(Message);
		} 
	}
	else
	{
		BroadcastMessage(Message);
	}
}

void UChatController::BroadcastMessage_Implementation(FChatMessage Message)
{
	SendMessage_Local(Message);
}


void UChatController::SendMessage_Local(const FChatMessage& Message)
{
#if !UE_BUILD_SHIPPING
	//check if tag not added in default tags list
	{
		const bool bChannelExist = DefaultChannelsData && DefaultChannelsData->ChannelGroups.ContainsByPredicate([&](const FChannelGroup& Group)
		{
			return Group.Channels.Contains(Message.Tag);
		});

		UE_CLOG(!bChannelExist, LogChatController, Error, TEXT("Chat channels not contain tag %s, message = %s"), *Message.Tag.ToString(), *Message.Message)
	}
#endif

	UE_LOG(LogChatController, Display, TEXT("%s: Message Received"), LOG_NET_ROLE_STR)
	OnReceiveMessage.Broadcast(Message);
}

void UChatController::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		ChatStyle = UChatStyleSave::InitSave(this, "ChatStyle", DefaultChannelsData);

		//todo maye need extract Damage info listener outside this class class? keep it there is not SOLID
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		DamageInfoListenerHandle = MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DAMAGE, this, &ThisClass::OnReceiveDamage);
		DamageInfoListenerHandle = MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DEATH, this, &ThisClass::OnReceiveDamage);
		DamageInfoListenerHandle = MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_HEAL, this, &ThisClass::OnReceiveDamage);
	}
}

void UChatController::GetRecipientsForMessage(const FChatMessage& Message, TArray<UChatProxy*>& OutRecipients)
{
	// Location message, get list of recipients from Location system.
	if (Message.Tag == TAG_CHAT_LOCATION)
	{
		//find location component in owner
		const auto Player = Message.From->GetPawn();
		const auto AreaListener = Player ? Player->FindComponentByClass<UAreaListenerComponent>() : nullptr;
		if (!AreaListener)
		{
			UE_LOG(LogChatController, Warning, TEXT("%s Send area message, but area listener not found in ownong pawn %s"),
				*Message.From->GetName(), *(Player ? Player->GetName() : FString("PawnNotValid")));
			return;
		}
		
		//get other actors in this location
		TArray<APawn*> Pawns;
		AreaListener->GetActorsInCurrentArea(Pawns);
		
		//get chat proxy from this actors
		for (const auto& Pawn : Pawns)
		{
			const auto ChatProxy = Pawn->IsPlayerControlled() ? Pawn->GetController()->FindComponentByClass<UChatProxy>() : nullptr;
			if (ChatProxy) OutRecipients.Add(ChatProxy);
		}
		return;
	}
	
}

void UChatController::OnReceiveDamage(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{
	if (!DamageInfo.IsValid()) return;

	//convert Damage info to message
	FChatMessage Message;
	Message.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	Message.Tag = DamageInfo.Tag;
	Message.From = DamageInfo.InstigatorState;
	Message.Message = DamageInfo.GetMessage();

	SendMessage_Local(Message);
}
