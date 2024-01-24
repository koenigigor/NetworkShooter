// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "ChatStyleSave.generated.h"

class UChatDefaultChannelsInfo;
class UDataTable;
struct FChatChannelInfo;

/** Saved info about single channel in tab */
USTRUCT(BlueprintType)
struct FChatChannelStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FontSize = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor TextColor = FLinearColor::Gray;
};

/** Saved info about single tab */
USTRUCT(BlueprintType)
struct FChatTabData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> ListenChannels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FChatChannelStyle> ChannelsStyles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowTime = true;

	/** Use default font size for all channels on tab */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseGeneralFontSize = true;
	
	/** Default font size on tab*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GeneralFontSize = 12;
};

/** Saved info about window settings */
USTRUCT(BlueprintType)
struct FChatWindowData
{
	GENERATED_BODY()

	/** Window location on viewport, in absolute space [0, 1] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AbsoluteCoordinate = FVector2D::ZeroVector;

	/** Widow size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Size = FVector2D(600.f, 300.f);

	/** Window opacity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RenderOpacity = 1.f;

	/** Show window if active tab receive message */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowWindowOnMessage = true;

	/** Show count of unread messages on tab */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowNewMessagesCount = true;

	/** Tab indexes on this window, sorted by row */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Tabs;
};

//Keep list of tabs, their info, viewport coordinate, tabs, tab rows, and their visual setup
UCLASS()
class NETWORKSHOTER_API UChatStyleSave : public USaveGame
{
	GENERATED_BODY()
public:
	/** This style save creates and exists only in ChatController class, so I can make getter for easiest access to it */
	UFUNCTION(BlueprintPure, DisplayName = "GetChatStyle", meta = (DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"))
	static UChatStyleSave* Get(UObject* WorldContextObject);
	
	static UChatStyleSave* InitSave(UObject* Outer, FString SaveSlot, UChatDefaultChannelsInfo* InDefaultChannelsInfo);
	void Save();

	//Notify window about change and save
	UFUNCTION(BlueprintCallable)
	void Apply(int32 WindowID);

	/** @brief register action culls when you perform related actions outside (in widget)
	 *	like if you delete tab call RegisterDeleteTab(), for save changes,
	 *	when yoy resize destroy window call RegisterDeleteWindow() etc. */
#pragma region RegisterActions

	/** Register tab in save when is added,
	 *	Notify chat window for create tab
	 *	@return NewTabID	*/
	UFUNCTION(BlueprintCallable, Category = "Chat")
	int32 RegisterNewTab(int32 WindowID, FChatTabData TabData);

	/** Register delete tab from save when is deleted */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void RegisterDeleteTab(int32 TabID);

	/** Move tab at new position */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void RegisterMoveTab(int32 TabID, int32 NewWindowID, int32 NewColumn);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	int32 RegisterNewWindow(FChatWindowData WindowData);

private:
	//todo manually delete window (now it delete if all tabs removed)
	//UFUNCTION(BlueprintCallable, Category = "Chat")
	void RegisterDeleteWindow(int32 WindowID);

public:
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void RegisterMoveWindow(int32 WindowID, FVector2D NewAbsPosition);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void RegisterResizeWindow(int32 WindowID, FVector2D NewAbsPosition, FVector2D NewWindowSize);

#pragma endregion RegisterActions

	/** Getters and setters for different properties */
#pragma region Settings
	
	/** Set new array of listen channels for this tab */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetListenChannelsInTab(int32 TabID, const TArray<FGameplayTag>& NewListenChannels);
	
	UFUNCTION(BlueprintPure, Category = "Chat")
	bool IsListenChannel(int32 TabID, FGameplayTag Channel) const;


	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetChannelStyle(int32 TabID, FGameplayTag Channel, FChatChannelStyle NewStyle);

	UFUNCTION(BlueprintPure, Category = "Chat")
	FLinearColor GetMessageColor(int32 TabID, FGameplayTag Channel) const;
	
	UFUNCTION(BlueprintPure, Category = "Chat")
	int32 GetFontSize(int32 TabID, FGameplayTag Channel) const;


	UFUNCTION(BlueprintCallable, Category = "Chat")
    void SetShowWindowOnMessage(int32 WindowID, bool bShow);

	UFUNCTION(BlueprintPure, Category = "Chat")
	bool IsShowWindowOnMessage(int32 WindowID) const;


	UFUNCTION(BlueprintCallable, Category = "Chat")
    void SetTabDefaultTextSize(int32 TabID, int32 InSize, bool bUseDefaultSize);
	
	UFUNCTION(BlueprintPure, Category = "Chat")
	int32 GetTabDefaultTextSize(int32 TabID) const;


	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetShowMessageTime(int32 TabID, bool bShowTime);

	UFUNCTION(BlueprintCallable, Category = "Chat")
    void SetTabName(int32 TabID, const FText& Name);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetShowMessagesCount(int32 WindowID, bool bShow);
	
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetWindowOpacity(int32 WindowID, float Opacity);

#pragma endregion Settings

	UFUNCTION(BlueprintPure, Category = "Chat")
	int32 GetWindowID(int32 TabID) const;	
	
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void ResetTabStyle(int32 TabID, bool bResetWindowSettings = true);

	//use for manually set window parameters, use carefully 
	void SetWindowDataFrom(int32 WindowID, FChatWindowData WindowData);
	void SetTabDataFrom(int32 TabID, FChatTabData TabData);
	
	UFUNCTION(BlueprintPure, Category = "Chat")
	const TMap<int32, FChatWindowData>& GetWindowsData() const { return WindowsData; }

	UFUNCTION(BlueprintPure, Category = "Chat")
	const FChatWindowData& GetWindowData(int32 WindowID) const;

	UFUNCTION(BlueprintPure, Category = "Chat")
	const FChatTabData& GetTabData(int32 TabID) const;

	/** Return Channels player can write to */
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void GetPlayerCanWriteChannels(TArray<FGameplayTag>& OutChannels) const;

	/** Return default info about channel */
	UFUNCTION(BlueprintPure, Category = "Chat")
	const FChatChannelInfo& GetChannelDefaultInfo(FGameplayTag Channel) const;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWindowStyleUpdateDelegate, int32, WindowID);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTabStyleUpdateDelegate, int32, TabID);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWindowTabStyleUpdateDelegate, int32, WindowID, int32, TabID);
	
	UPROPERTY(BlueprintAssignable, Transient)
	FWindowStyleUpdateDelegate OnWindowStyleUpdate;

	//notify chat window create new tab 
	UPROPERTY(BlueprintAssignable, Transient)
	FWindowTabStyleUpdateDelegate OnCreateNewTab;
	
	//notify chat window delete teb 
	UPROPERTY(BlueprintAssignable, Transient)
	FWindowTabStyleUpdateDelegate OnDeleteTab;

	
	UFUNCTION(BlueprintPure, Category = "Chat")
	int32 GetMinFontSize() const { return 12; };
	UFUNCTION(BlueprintPure, Category = "Chat")
	int32 GetMaxFontSize() const { return 23; };
	
protected:
	int32 GenerateNewTabID() const;
	int32 GenerateNewWindowID() const;

	UPROPERTY()
	TMap<int32, FChatWindowData> WindowsData;
	UPROPERTY()
	TMap<int32, FChatTabData> TabsData;

	UPROPERTY()
	FString SlotName;

	// data asset with default info about message channels, set by init save
	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<UChatDefaultChannelsInfo> DefaultChannelsInfo = nullptr;
	
	/**
	//input todo
	FVector2D InputBoxAbsoluteCoordinate;
	int32 InputBoxFontSize = 12;
	
	*/
};
