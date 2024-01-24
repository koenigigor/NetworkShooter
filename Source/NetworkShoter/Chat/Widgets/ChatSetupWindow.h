// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat/ChatStyleSave.h"
#include "ChatSetupWindow.generated.h"

class UCheckBox;
class USlider;
class UComboBox;
class UTextBlock;
class UChatStyleSave;
class UButton;
class UEditableText;
class UComboBoxString;

/** Window for edit ChatStyle */
UCLASS(Abstract)
class NETWORKSHOTER_API UChatSetupWindow : public UUserWidget
{
	GENERATED_BODY()
public:
	//UWChatSetupWindow();
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void InitWindow(int32 WindowID, int32 TabID);
	UFUNCTION(BlueprintCallable)
	void ApplyStyle();
	UFUNCTION(BlueprintCallable)
	void ResetBackupParameters();

	//tmp
	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitWindow();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ApplyStyle();

	
#pragma region WidgetEventHandles
protected:
	UFUNCTION()
    void ShowWindowOnMessageHandle(bool bBIsChecked);

	UFUNCTION()
	void ShowNewMessagesCountHandle(bool bBIsChecked);

	UFUNCTION()
	void WindowOpacityHandle(float Value);
	
	UFUNCTION()
	void ShowTimeInMessageHandle(bool bBIsChecked);

	UFUNCTION()
	void TabFontSizeHandle(float Value);
	
	UFUNCTION()
	void UseTabFontSizeHandle(bool bBIsChecked);

	UFUNCTION()
	void ChangeTabHandle(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void EditTabNameHandle(const FText& Text);

	
	UFUNCTION()
	void ApplyStyleHandle();
	UFUNCTION()
	void ToDefaultsHandle();
#pragma endregion WidgetEventHandles
	
protected:
	FChatTabData BackupTabData;
	FChatWindowData BackupWindowData;

	UPROPERTY(BlueprintReadWrite)
	int32 WindowID = INDEX_NONE;
	UPROPERTY(BlueprintReadWrite)
	int32 TabID = INDEX_NONE;

	// true if editing newly created tab (if not apply in must be delete)
	bool bPreviewTab = false;

	// true when we edit tab (for not reset defaults twice (on button and close window))
	bool bEditing = false;

#pragma region Widgets
///
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Apply_Button = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Defaults_Button = nullptr;
	
/// Window settings
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* ShowWindowOnMessage_CheckBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* ShowNewMessagesCount_CheckBox = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* WindowOpacity_Slider = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* WindowOpacity_Text = nullptr;

/// Tab settings
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* ShowTimeInMessage_CheckBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* TabFontSize_Slider = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TabFontSize_Text = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* UseTabFontSize_CheckBox = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* TabName_EditText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxString* ChangeTab_ComboBox = nullptr;

#pragma endregion Widgets
	
	TWeakObjectPtr<UChatStyleSave> ChatStyle;
};



