// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/ChatController.h"

#include "Game/NSPlayerState.h"


UChatController::UChatController()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
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


