// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/Widgets/ChatWindow.h"

#include "ChatTabButton.h"
#include "ChatScrollBox.h"
#include "ChatSetupWindow.h"
#include "Selection.h"
#include "Chat/ChatController.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Misc/TavaUILib.h"

DEFINE_LOG_CATEGORY_STATIC(LogChatWindow, All, All);


void UChatWindow::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (const auto ChatSave = UChatStyleSave::Get(GetOwningPlayer()))
	{
		ChatSave->OnCreateNewTab.AddDynamic(this, &ThisClass::OnCreateNewTab);
		ChatSave->OnWindowStyleUpdate.AddDynamic(this, &ThisClass::OnStyleUpdate);
	}
}

void UChatWindow::NativeConstruct()
{
	Super::NativeConstruct();

	UChatController::Get(this)->OnReceiveMessage.AddDynamic(this, &ThisClass::OnReceiveMessage);
}

void UChatWindow::SelectTab(UChatTabButton* Tab)
{
	if (SelectedTab)
		SelectedTab->SetSelection(false);

	SelectedTab = Tab;
	SelectedTab->SetSelection(true);
	ChatSwitcher->SetActiveWidget(Tabs.FindChecked(Tab));
}

void UChatWindow::AddNewTab(int32 TabID, int32 Column)
{
	const auto Tab = CreateWidget<UChatTabButton>(GetOwningPlayer(), TabClass);
	Tab->TabID = TabID;
	Tab->LoadStyle();
	
	const auto MessageBox = CreateWidget<UChatScrollBox>(GetOwningPlayer(), MessageBoxClass);
	MessageBox->TabID = TabID;

	AddTabAtColumn(Column, Tab, MessageBox);
}

void UChatWindow::AddTabAtColumn(int32 Column, UChatTabButton* Tab, UChatScrollBox* MessageBox)
{
	ShiftTabs(Column, true);

	const auto GridSlot = TabsGrid->AddChildToUniformGrid(Tab, 0, Column);	
	GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	ChatSwitcher->AddChild(MessageBox);

	Tabs.Add(Tab, MessageBox);
	Tab->OnClickTab.AddDynamic(this, &ThisClass::SelectTab);
	Tab->OnDeleteTab.AddDynamic(this, &ThisClass::DeleteTab);
}

UChatScrollBox* UChatWindow::RemoveTab(UChatTabButton* Tab)
{
	if (!(Tab && Tab->Slot)) return nullptr;
	if (!Tabs.Contains(Tab))
	{
#if !NO_LOGGING
		const auto ChatStyle = UChatStyleSave::Get(GetOwningPlayer());
		const auto TabName = ChatStyle->GetTabData(Tab->TabID).TabName.ToString();
		UE_LOG(LogChatWindow, Warning, TEXT("Remove tab - Tab not belong window, WindowID = %d, TabName = %s"), WindowID, *TabName)
#endif
		
		return nullptr;
	}

	const auto MessageBox = Tabs.FindAndRemoveChecked(Tab);
	const auto bActiveTab = ChatSwitcher->GetActiveWidget() == MessageBox;

	// Move right columns
	const auto Column = CastChecked<UUniformGridSlot>(Tab->Slot)->GetColumn();
	ShiftTabs(Column, false);

	// Remove tab
	Tab->OnClickTab.RemoveAll(this);	//todo
	Tab->OnDeleteTab.RemoveAll(this);
	Tab->RemoveFromParent();
	if (MessageBox) MessageBox->RemoveFromParent();		//todo make message box valid all time, create fake message box for drag tab?

	// Select new tab or remove window
	if (bActiveTab)
	{
		if (TabsGrid->GetChildrenCount() != 0)
		{
			SelectTab(Cast<UChatTabButton>(TabsGrid->GetChildAt(0)));
		}
		else
		{
			RemoveFromParent();
		}
	}

	return MessageBox;
}

int32 UChatWindow::GetColumnUnderCursor() const
{
	if (Tabs.IsEmpty()) return 0;
	
	const auto& TabIt = Tabs.CreateConstIterator();
	const auto Tab = TabIt.Key();
	const auto GridSlot = CastChecked<UUniformGridSlot>(Tab->Slot);
	const auto AbsPosition = UTavaUILib::GetAbsMousePositionOnWidget(Tab, TabsGrid->GetSlotPadding());

	// use uniform grid, so we can simple calculate offset from any column
	int32 ColumnUnderCursor = GridSlot->GetColumn() + FMath::FloorToInt(AbsPosition.X);
		
	// clamp to columns + 1
	ColumnUnderCursor = FMath::Clamp(ColumnUnderCursor, 0, Tabs.Num());
	
	return ColumnUnderCursor;
}

void UChatWindow::SwapTabs(int32 FirstColumn, int32 SecondColumn)
{
	UUniformGridSlot* FirstSlot = nullptr;
	UUniformGridSlot* SecondSlot = nullptr;
	
	for (const auto& [Tab, MessageBox] : Tabs)
	{
		const auto GridSlot = CastChecked<UUniformGridSlot>(Tab->Slot);
		if (GridSlot->GetColumn() == FirstColumn)
			FirstSlot = GridSlot;
		if (GridSlot->GetColumn() == SecondColumn)
			SecondSlot = GridSlot;
	}

	if (FirstSlot && SecondSlot)
	{
		FirstSlot->SetColumn(SecondColumn);
		SecondSlot->SetColumn(FirstColumn);
	}	
}

void UChatWindow::LoadStyle()
{
	const auto ChatStyle = UChatStyleSave::Get(GetOwningPlayer());
	const auto& WindowData =  ChatStyle->GetWindowData(WindowID);

	/// Load size
	WindowSizeBox->SetWidthOverride(WindowData.Size.X);
	WindowSizeBox->SetHeightOverride(WindowData.Size.Y);

	/// Load tabs
	TabsGrid->ClearChildren();
	ChatSwitcher->ClearChildren();
	
	int32 Column = 0; 
	for (const int32 TabID : WindowData.Tabs)
	{
		AddNewTab(TabID, Column);
		++Column;
	}
	SelectTab(Cast<UChatTabButton>(TabsGrid->GetChildAt(0)));

	
	LoadRuntimeStyle();
}

void UChatWindow::LoadRuntimeStyle()
{
	const auto ChatStyle = UChatStyleSave::Get(GetOwningPlayer());
	const auto& WindowData =  ChatStyle->GetWindowData(WindowID);

	MaxRenderOpacity = WindowData.RenderOpacity;
	SetRenderOpacity(MaxRenderOpacity);
}

void UChatWindow::ShiftTabs(int32 Column, bool bToRightSide)
{
	for (const auto& [Tab, MessageBox] : Tabs)
	{
		const auto GridSlot = CastChecked<UUniformGridSlot>(Tab->Slot);
		const auto IterColumn = GridSlot->GetColumn();
		if (IterColumn >= Column)
		{
			GridSlot->SetColumn(IterColumn + (bToRightSide ? 1 : -1));
		}
	}
}

void UChatWindow::OnReceiveMessage(const FChatMessage& Message)
{
	// React on message event
	if (SelectedTab)
	{
		const auto ChatSave = UChatStyleSave::Get(this);
		check(ChatSave);
		if (ChatSave->IsListenChannel(SelectedTab->TabID, Message.Tag))
		{
			ReactOnMessage();
		}
	}
}

void UChatWindow::DeleteTab(UChatTabButton* Tab)
{
	RemoveTab(Tab);
	
	const auto ChatSave = UChatStyleSave::Get(GetOwningPlayer());
	ChatSave->RegisterDeleteTab(Tab->TabID);
}

void UChatWindow::OnCreateNewTab(int32 InWindowID, int32 TabID)
{
	if (WindowID == InWindowID)
	{
		const auto LastColumn = TabsGrid->GetChildrenCount();
		AddNewTab(TabID, LastColumn);
	}
}

void UChatWindow::OnStyleUpdate(int32 InWindowID)
{
	if (InWindowID == WindowID)
	{
		LoadRuntimeStyle();
	}
}
