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
	FString InstigatorName;
	FString TargetName;
	FString CauserName;
	/*
	//Prepare DamageInstigator name
	if (DamageInstigator)
	{
		if (DamageInstigator -> GetPlayerState<APlayerState>())
		{
			InstigatorName = DamageInstigator -> GetPlayerState<APlayerState>() -> GetPlayerName();
		}
		else
		{
			InstigatorName = DamageInstigator -> GetName();
		}
	}

	//Prepare DamageReceiver name
	if (DamagedActor->GetInstigatorController())
	{
		if (DamagedActor->GetInstigatorController()->GetPlayerState<APlayerState>())
		{
			DamagedActorName = DamagedActor->GetInstigatorController()->GetPlayerState<APlayerState>()->GetPlayerName();
		}
		else
		{
			DamagedActorName = DamagedActor->GetInstigatorController()->GetName();
		}
	}
	else
	{
		DamagedActorName = DamagedActor->GetName();
	}
	
	if (DamageInfo.DamageCauser) //first spawned actor on equipped weapon
	{
		CauserName = DamageInfo.DamageCauser->GetName();
	}
	else
	{
		CauserName = "No DamageCauser Name";
	}
	*/
	

}

void UChatController::ReceiveMessage_Implementation(const FString& Message, ANSPlayerState* FromWho)
{
	UE_LOG(LogTemp, Warning, TEXT("Chat: Message Received"))
	
	//notify chat widget
	MessageReceive.Broadcast(FromWho, Message);
}


