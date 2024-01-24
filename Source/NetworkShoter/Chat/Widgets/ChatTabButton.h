// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatTabButton.generated.h"

class UTextBlock;
class UBorder;
struct FChatMessage;

/** Base class of tab button for ChatWindow widget */
UCLASS(Abstract)
class NETWORKSHOTER_API UChatTabButton : public UUserWidget
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickTab, UChatTabButton*, Tab);
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, meta=( ExposeOnSpawn="true" ))
	int32 TabID;

	UFUNCTION(BlueprintCallable)
	void SetSelection(bool bSelect);

	UFUNCTION(BlueprintPure, DisplayName="IsSelected")
	bool IsTabSelected() const { return bSelect; };

	UFUNCTION(BlueprintCallable)
	void LoadStyle();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FClickTab OnClickTab;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FClickTab OnDeleteTab;
	
protected:
	bool bSelect = false;
	bool bShowMessageCount = false;
	int32 NewMessageCount = 0;

    void UpdateMessageCountWidget();
	
	UFUNCTION()
	void OnReceiveMessage(const FChatMessage& Message);

	UFUNCTION()
	void OnStyleUpdate(int32 WindowID);

protected:
/// Widgets	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TabName = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* NewMessageCountBorder = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NewMessageCountText = nullptr;	

protected:
/// BP Events
	/** Event if tab handle this message */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnReceiveMessage")
	void BP_OnReceiveMessage(const FChatMessage& Message);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnToggleSelection")
	void BP_ToggleSelection(bool Select);
};

