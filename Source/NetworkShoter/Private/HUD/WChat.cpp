// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WChat.h"

#include "ChatController.h"
#include "Game/NSPlayerState.h"
#include "Game/PCNetShooter.h"
#include "GameFramework/GameStateBase.h"

void UWChat::NativeConstruct()
{
	Super::NativeConstruct();

	ChatController = GetWorld()->GetGameState()->FindComponentByClass<UChatController>();

	if (ChatController)
	{
		ChatController->MessageReceive.AddDynamic(this, &UWChat::OnReceiveMessage);
	}
}

void UWChat::NativeDestruct()
{
	Super::NativeDestruct();

	if (ChatController)
	{
		ChatController->MessageReceive.RemoveDynamic(this, &UWChat::OnReceiveMessage);
	}
}

void UWChat::OnReceiveMessage(ANSPlayerState* FromPlayer, FString Message)
{
	BP_OnReceiveMessage(FromPlayer, Message);
}
