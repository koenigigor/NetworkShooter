// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWindow.generated.h"

class UChatTabButton;
class UChatScrollBox;
class UWidgetSwitcher;
class UUniformGridPanel;
class USizeBox;
struct FChatMessage;

/** Base class of chat window */
UCLASS(Abstract)
class NETWORKSHOTER_API UChatWindow : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<UChatTabButton> TabClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<UChatScrollBox> MessageBoxClass;
	

	UPROPERTY(BlueprintReadWrite, meta=( ExposeOnSpawn="true" ))
	int32 WindowID = -1;

	UFUNCTION(BlueprintCallable, Category = "Tab")
	void SelectTab(UChatTabButton* Tab);

	UFUNCTION(BlueprintCallable, Category = "Tab")
	void AddNewTab(int32 TabID, int32 Column);	
	
	UFUNCTION(BlueprintCallable, Category = "Tab")
	void AddTabAtColumn(int32 Column, UChatTabButton* Tab, UChatScrollBox* MessageBox);

	UFUNCTION(BlueprintCallable, Category = "Tab")
	UChatScrollBox* RemoveTab(UChatTabButton* Tab);

	/** Return tab button column under mouse position,
	 *  clamp 0 to last column +1 */
	UFUNCTION(BlueprintPure, Category = "Tab")
	int32 GetColumnUnderCursor() const;

	/** Swap tabs in columns */
	UFUNCTION(BlueprintCallable, Category = "Tab")
    void SwapTabs(int32 FirstColumn, int32 SecondColumn);

	/** Load all window settings */
	UFUNCTION(BlueprintCallable, Category = "Window")
	void LoadStyle();

	/** Load window settings who can change at runtime */
	UFUNCTION(BlueprintCallable, Category = "Window")
	void LoadRuntimeStyle();
	
protected:
	/** Shift tab buttons inside grid panel, column included */
	void ShiftTabs(int32 Column, bool bToRightSide);
	
	UFUNCTION()
	void OnReceiveMessage(const FChatMessage& Message);

	UFUNCTION()
	void DeleteTab(UChatTabButton* Tab);

	UFUNCTION()
	void OnCreateNewTab(int32 InWindowID, int32 TabID);

	UFUNCTION()
	void OnStyleUpdate(int32 InWindowID);
	
protected:
	/** Map of tab buttons and their messages box */
	UPROPERTY(BlueprintReadWrite)
	TMap<UChatTabButton*, UChatScrollBox*> Tabs;

	UPROPERTY(BlueprintReadWrite)
	UChatTabButton* SelectedTab = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float MaxRenderOpacity = 1.f;

	
/// BP Events	
	/** Event when selected tab got message */
	UFUNCTION(BlueprintImplementableEvent)
	void ReactOnMessage();

	
/// Widgets
	/** Message boxes (ChatScrollBox) container */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	USizeBox* WindowSizeBox = nullptr;
		
	/** Message boxes (ChatScrollBox) container */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UWidgetSwitcher* ChatSwitcher = nullptr;

	/** Tabs (UChatTabButton) container */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UUniformGridPanel* TabsGrid = nullptr;
};
