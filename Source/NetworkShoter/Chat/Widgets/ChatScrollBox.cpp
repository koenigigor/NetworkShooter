// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatScrollBox.h"

#include "Chat/ChatStyleSave.h"
#include "../ChatController.h"
#include "Game/NSPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogChatMessagesBox, All, All);


void UChatScrollBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (const auto ChatController = UChatController::Get(this))
	{
		ChatController->OnReceiveMessage.AddDynamic(this, &ThisClass::OnReceiveMessage);
	}
}

void UChatScrollBox::NativeDestruct()
{
	Super::NativeDestruct();

	if (const auto ChatController = UChatController::Get(this))
	{
		ChatController->OnReceiveMessage.RemoveDynamic(this, &ThisClass::OnReceiveMessage);
	}
}

void UChatScrollBox::OnReceiveMessage(const FChatMessage& Message)
{
	if (const auto ChatController = UChatController::Get(this))
	{
		if (const auto Style = ChatController->GetChatStyle())
		{
			if (Style->IsListenChannel(TabID, Message.Tag))
			{
				BP_OnReceiveMessage(Message);
			}
		}
	}
}
