// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/ChatController.h"

#include "Game/NSPlayerState.h"
#include "GameFramework/GameplayMessageSubsystem.h"


UChatController::UChatController()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void UChatController::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(NSTag::System::Damage(), this, &ThisClass::OnReceiveDamage);
	ListenerHandle = MessageSubsystem.RegisterListener(NSTag::System::Death(), this, &ThisClass::OnReceiveDamage);
	//ListenerHandle = MessageSubsystem.RegisterListener(NSTag::System::Heal(), this, &ThisClass::OnReceiveDamage);
}

void UChatController::OnReceiveDamage(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{
	//replicate info to clients
	BroadcastDamageInfo(DamageInfo);
}

void UChatController::BroadcastDamageInfo_Implementation(FDamageInfo DamageInfo)
{	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	//send DamageInfo in chat
	if (DamageInfo.Tag == NSTag::System::Damage())
	{
		
	}

	//send kill Info in KillFeed
	if (DamageInfo.Tag == NSTag::System::Death())
	{
		MessageSubsystem.BroadcastMessage(NSTag::Chat::Damage(), DamageInfo);
	}
}

void UChatController::SendMessage_Player(const FString& Message, ANSPlayerState* FromWho)
{
	//message analysis

	//add message in log

	//send message to clients
	ReceiveMessage(Message, FromWho);
}

void UChatController::ReceiveMessage_Implementation(const FString& Message, ANSPlayerState* FromWho)
{
	UE_LOG(LogTemp, Warning, TEXT("Chat: Message Received"))
	
	//notify chat widget
	MessageReceive.Broadcast(FromWho, Message);
}


