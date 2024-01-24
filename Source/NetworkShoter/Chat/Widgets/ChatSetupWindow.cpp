// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSetupWindow.h"

#include "Components/Button.h"
#include "Components/ComboBox.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"


/*
UWChatSetupWindow::UWChatSetupWindow()
{
	if (WindowOpacity_Slider)
	{
		WindowOpacity_Slider->SetMinValue(0.2f);
		WindowOpacity_Slider->SetMaxValue(1.f);
		WindowOpacity_Slider->SetStepSize(0.01f);
	}
	if (TabFontSize_Slider)
	{
		WindowOpacity_Slider->SetMinValue(8);
		WindowOpacity_Slider->SetMaxValue(30);
		WindowOpacity_Slider->SetStepSize(1.f);		
	}

	TabName_EditText->SetHintText("EnterTabName");
}
*/

void UChatSetupWindow::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(ShowWindowOnMessage_CheckBox);
	check(ShowNewMessagesCount_CheckBox)
	check(WindowOpacity_Slider)
	check(ShowTimeInMessage_CheckBox)
	check(TabFontSize_Slider)
	check(UseTabFontSize_CheckBox)
	check(ChangeTab_ComboBox)
	check(Apply_Button)
	check(Defaults_Button)

	ShowWindowOnMessage_CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::ShowWindowOnMessageHandle);
	ShowNewMessagesCount_CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::ShowNewMessagesCountHandle);
	WindowOpacity_Slider->OnValueChanged.AddDynamic(this, &ThisClass::WindowOpacityHandle);

	ShowTimeInMessage_CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::ShowTimeInMessageHandle);
	TabFontSize_Slider->OnValueChanged.AddDynamic(this, &ThisClass::TabFontSizeHandle);
	UseTabFontSize_CheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::UseTabFontSizeHandle);
	TabName_EditText->OnTextChanged.AddDynamic(this, &ThisClass::EditTabNameHandle);

	ChangeTab_ComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::ChangeTabHandle);
	
	Apply_Button->OnClicked.AddDynamic(this, &ThisClass::ApplyStyleHandle);
	Defaults_Button->OnClicked.AddDynamic(this, &ThisClass::ToDefaultsHandle);
}

void UChatSetupWindow::NativeDestruct()
{
	Super::NativeDestruct();

	ResetBackupParameters();
}

void UChatSetupWindow::InitWindow(int32 InWindowID, int32 InTabID)
{
	const auto ChatStylePtr = UChatStyleSave::Get(this);
	check(ChatStylePtr);
	ChatStyle = MakeWeakObjectPtr(ChatStylePtr);

	WindowID = InWindowID;
	TabID = InTabID;
	bPreviewTab = false;
	bEditing = true;
	
	//create new tab if none
	if (TabID == INDEX_NONE)
	{
		TabID = ChatStyle->RegisterNewTab(WindowID, FChatTabData());
		bPreviewTab = true;
	}

	//save backup data for revert changes
	BackupTabData = ChatStyle->GetTabData(TabID);
	BackupWindowData = ChatStyle->GetWindowData(WindowID);

	//Update widget parameters
	{
		check(ShowWindowOnMessage_CheckBox);
		ShowWindowOnMessage_CheckBox->SetIsChecked(BackupWindowData.bShowWindowOnMessage);
	
		check(ShowNewMessagesCount_CheckBox)
		ShowNewMessagesCount_CheckBox->SetIsChecked(BackupWindowData.bShowNewMessagesCount);

		check(WindowOpacity_Slider)
		WindowOpacity_Slider->SetValue(BackupWindowData.RenderOpacity);
		if (WindowOpacity_Text)
		{
			FNumberFormattingOptions Options;
			Options.MaximumFractionalDigits = 0;
			WindowOpacity_Text->SetText(FText::AsNumber(BackupWindowData.RenderOpacity * 100.f, &Options));
		}

		check(ShowTimeInMessage_CheckBox);
		ShowTimeInMessage_CheckBox->SetIsChecked(BackupTabData.bShowTime);

		TabFontSize_Slider->SetValue(BackupTabData.GeneralFontSize);
		UseTabFontSize_CheckBox->SetIsChecked(BackupTabData.bUseGeneralFontSize);
		if (TabFontSize_Text)
		{
			FNumberFormattingOptions Options;
			Options.MaximumFractionalDigits = 0;
			TabFontSize_Text->SetText(FText::AsNumber(BackupTabData.GeneralFontSize, &Options));
		}

		check(TabName_EditText);
		TabName_EditText->SetText(BackupTabData.TabName);

		//init change tab combo box
		check(ChangeTab_ComboBox);
		ChangeTab_ComboBox->ClearOptions();
		for (auto Tab : BackupWindowData.Tabs)
		{
			if (Tab != TabID)
				ChangeTab_ComboBox->AddOption(FString::FromInt(Tab));
		}
		ChangeTab_ComboBox->AddOption(FString::FromInt(INDEX_NONE));
	}

	BP_InitWindow();
}

void UChatSetupWindow::ApplyStyle()
{
	if (!ChatStyle.IsValid()) return;

	BP_ApplyStyle();
	ChatStyle->Apply(WindowID);

	bEditing = false;
	
	//update backup variables and start editing again
	InitWindow(WindowID, TabID);	
}

void UChatSetupWindow::ResetBackupParameters()
{
	if (!bEditing) return;
	bEditing = false;
	
	if (!ChatStyle.IsValid()) return;
	if (bPreviewTab)
	{
		ChatStyle->RegisterDeleteTab(TabID);
		RemoveFromParent();
		return;
	}

	ChatStyle->SetWindowDataFrom(WindowID, BackupWindowData);
	ChatStyle->SetTabDataFrom(TabID, BackupTabData);
	ChatStyle->Apply(WindowID);
}

#pragma region WidgetEventHandles

void UChatSetupWindow::ShowWindowOnMessageHandle(bool bBIsChecked)
{
	if (ChatStyle.IsValid())
		ChatStyle->SetShowWindowOnMessage(WindowID, bBIsChecked);
}

void UChatSetupWindow::ShowNewMessagesCountHandle(bool bBIsChecked)
{
	if (ChatStyle.IsValid())
		ChatStyle->SetShowMessagesCount(WindowID, bBIsChecked);
}

void UChatSetupWindow::WindowOpacityHandle(float Value)
{
	if (!ChatStyle.IsValid()) return;

	ChatStyle->SetWindowOpacity(WindowID, Value);

	if (WindowOpacity_Text)
	{
		FNumberFormattingOptions Options;
		Options.MaximumFractionalDigits = 0;
		WindowOpacity_Text->SetText(FText::AsNumber(Value * 100.f, &Options));
	}
}

void UChatSetupWindow::ShowTimeInMessageHandle(bool bBIsChecked)
{
	if (!ChatStyle.IsValid()) return;

	ChatStyle->SetShowMessageTime(TabID, bBIsChecked);
}

void UChatSetupWindow::TabFontSizeHandle(float Value)
{
	if (!ChatStyle.IsValid()) return;

	if (TabFontSize_Text)
	{
		FNumberFormattingOptions Options;
		Options.MaximumFractionalDigits = 0;
		TabFontSize_Text->SetText(FText::AsNumber(Value, &Options));
	}
	
	ChatStyle->SetTabDefaultTextSize(
		TabID,
		FMath::TruncToInt(Value),
		UseTabFontSize_CheckBox->IsChecked());
}

void UChatSetupWindow::UseTabFontSizeHandle(bool bBIsChecked)
{
	if (!ChatStyle.IsValid()) return;

	ChatStyle->SetTabDefaultTextSize(
		TabID,
		FMath::TruncToInt(TabFontSize_Slider->GetValue()),
		bBIsChecked);
}

void UChatSetupWindow::ChangeTabHandle(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::OnMouseClick)
	{
		InitWindow(WindowID, FCString::Atoi(*SelectedItem));
	}
}

void UChatSetupWindow::EditTabNameHandle(const FText& Text)
{
	ChatStyle->SetTabName(TabID, Text);
}

void UChatSetupWindow::ToDefaultsHandle()
{
	if (ChatStyle.IsValid())
		ChatStyle->ResetTabStyle(TabID, true);
	
	RemoveFromParent();
}

void UChatSetupWindow::ApplyStyleHandle()
{
	ApplyStyle();
	RemoveFromParent();
}

#pragma endregion WidgetEventHandles
