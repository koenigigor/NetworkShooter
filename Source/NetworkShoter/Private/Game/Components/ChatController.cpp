// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/ChatController.h"

#include "Game/NSPlayerState.h"
#include "NSStructures.h"


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

void UChatController::SendDamageInfo(FDamageInfo DamageInfo)
{
	FString InstigatorName = "NoInstigator";
	FString TargetName = "";
	FString CauserName = "NoCauser";
	
	if (DamageInfo.Instigator)
	{
		//intend instigator is controller
		if (const auto DamageInstigator = Cast<AController>(DamageInfo.Instigator))
		{
			InstigatorName = DamageInstigator -> PlayerState -> GetPlayerName();
		}
		else
		{
			InstigatorName = DamageInfo.Instigator -> GetName();
		}
	}
	
	if (DamageInfo.Target)
	{
		TargetName = DamageInfo.Target->GetInstigatorController()->PlayerState->GetPlayerName();
	}
	
	if (DamageInfo.DamageCauser) //first spawned actor on equipped weapon
	{
		CauserName = DamageInfo.DamageCauser->GetName();
	}
	else
	{
		CauserName = "No DamageCauser Name";
	}

	FDamageInfoChat Message;
	Message.MessageTag;
	Message.Instigator;
	Message.Target;
	Message.DamageCauser;
	Message.Damage;
	Message.Time;

	//ReceiveMessage(MESSAGE_DAMAGE, )
}

void UChatController::ReceiveMessage_Implementation(const FString& Message, ANSPlayerState* FromWho)
{
	UE_LOG(LogTemp, Warning, TEXT("Chat: Message Received"))
	
	//notify chat widget
	MessageReceive.Broadcast(FromWho, Message);
}


