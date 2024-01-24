// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/Widgets/ChatTabButton.h"

#include "Chat/ChatController.h"
#include "Chat/ChatStyleSave.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UChatTabButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UChatController::Get(this)->OnReceiveMessage.AddDynamic(this, &ThisClass::OnReceiveMessage);
	
}

void UChatTabButton::NativeConstruct()
{
	Super::NativeConstruct();

	UChatStyleSave::Get(this)->OnWindowStyleUpdate.AddDynamic(this, &ThisClass::OnStyleUpdate);
}

void UChatTabButton::NativeDestruct()
{
	Super::NativeDestruct();

	UChatStyleSave::Get(this)->OnWindowStyleUpdate.RemoveDynamic(this, &ThisClass::OnStyleUpdate);
}

void UChatTabButton::OnStyleUpdate(int32 WindowID)
{
	LoadStyle();
}

void UChatTabButton::SetSelection(bool Select)
{
	if (bSelect == Select) return; // Already selected

	bSelect = Select;

	if (bSelect)
	{
		NewMessageCount = 0;
		UpdateMessageCountWidget();
	}
	
	BP_ToggleSelection(bSelect);
}

void UChatTabButton::LoadStyle()
{
	const auto ChatStyle = UChatStyleSave::Get(this);

	const auto& WindowInfo = ChatStyle->GetWindowData(ChatStyle->GetWindowID(TabID));
	const auto& TabInfo = ChatStyle->GetTabData(TabID);

	TabName->SetText(TabInfo.TabName);

	bShowMessageCount = WindowInfo.bShowNewMessagesCount;
	UpdateMessageCountWidget();
}

void UChatTabButton::OnReceiveMessage(const FChatMessage& Message)
{
	const auto ChatSave = UChatStyleSave::Get(this);
	check(ChatSave);

	if (!ChatSave->IsListenChannel(TabID, Message.Tag)) return;

	if (!bSelect)
	{
		++NewMessageCount;
		UpdateMessageCountWidget();
	}
	
	BP_OnReceiveMessage(Message);
}

void UChatTabButton::UpdateMessageCountWidget()
{
	if (bShowMessageCount && NewMessageCount > 0)
	{
		const auto CountText = NewMessageCount < 100
			? FText::AsNumber(NewMessageCount, &FNumberFormattingOptions::DefaultNoGrouping())
			: FText::FromString("99+");

		NewMessageCountText->SetText(CountText);
		NewMessageCountBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		NewMessageCountBorder->SetVisibility(ESlateVisibility::Collapsed);		
	}
}

