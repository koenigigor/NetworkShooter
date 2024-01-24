// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatStyleSave.h"

#include "ChatController.h"
#include "ChatDefaultChannelsInfo.h"
#include "Kismet/GameplayStatics.h"
#include "NSStructures.h"

DEFINE_LOG_CATEGORY_STATIC(LogChatStyle, All, All);

#define LOCTEXT_NAMESPACE "Chat"

UChatStyleSave* UChatStyleSave::Get(UObject* WorldContextObject)
{
	const auto ChatController = UChatController::Get(WorldContextObject);
	
	if (!ChatController)
	{
		UE_LOG(LogChatStyle, Warning, TEXT("Try get instance, but chat controller is invalid"))
	}
	else if (!ChatController->GetChatStyle())
	{
		UE_LOG(LogChatStyle, Warning, TEXT("Style instance not initilized, it must be done from Chat controller"))
	}
	
	return ChatController ? ChatController->GetChatStyle() : nullptr;
}

UChatStyleSave* UChatStyleSave::InitSave(UObject* Outer, FString SaveSlot, UChatDefaultChannelsInfo* InDefaultChannelsInfo)
{
	ensure(Outer);
	ensure(!SaveSlot.IsEmpty());
	UE_LOG(LogChatStyle, Display, TEXT("InitSave..."))

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, 0))
	{
		const auto Save = UGameplayStatics::LoadGameFromSlot(SaveSlot, 0);
		if (const auto StyleSave = Cast<UChatStyleSave>(Save))
		{
			UE_LOG(LogChatStyle, Display, TEXT("...Loaded"))
			StyleSave->SlotName = SaveSlot;                                         //without in first test after launch name is empty
			StyleSave->Rename(*StyleSave->GetName(), Outer, StyleSave->GetFlags()); //set outer
			StyleSave->DefaultChannelsInfo = InDefaultChannelsInfo;
			return StyleSave;
		}
	}

	const auto StyleSave = NewObject<UChatStyleSave>(Outer);
	StyleSave->SlotName = SaveSlot;
	StyleSave->DefaultChannelsInfo = InDefaultChannelsInfo;

	//setup default window (2 tab General and system)
	{
		StyleSave->TabsData =
		{
			{
				0,
				FChatTabData{
					LOCTEXT("GeneralTabName", "General"),
					{TAG_CHAT_GENERAL, TAG_CHAT_GROUP},
					{},
					false,
					true,
					12
				}
			},
			{
				1,
				FChatTabData{
					LOCTEXT("SystemTabName", "System"),
					{TAG_MESSAGE_SYSTEM_DAMAGE, TAG_MESSAGE_SYSTEM_HEAL, TAG_MESSAGE_SYSTEM_DEATH},
					{},
					true,
					true,
					8
				}}
		};

		StyleSave->WindowsData =
		{
			{
				0,
				FChatWindowData{
					FVector2D(0.1f, 0.8f),
					FVector2D(600.f, 300.f),
					1.f,
					true,
					true,
					{0, 1}
				}
			}
		};
	}

	StyleSave->Save();
	UE_LOG(LogChatStyle, Display, TEXT("...Created"))

	return StyleSave;
}

#undef LOCTEXT_NAMESPACE

void UChatStyleSave::Save()
{
	ensure(!SlotName.IsEmpty());

	UGameplayStatics::SaveGameToSlot(this, SlotName, 0);
}

void UChatStyleSave::Apply(int32 WindowID)
{
	OnWindowStyleUpdate.Broadcast(WindowID);
	Save();
}

int32 UChatStyleSave::RegisterNewTab(int32 WindowID, FChatTabData TabData)
{
	if (!WindowsData.Contains(WindowID))
	{
		UE_LOG(LogChatStyle, Warning, TEXT("Register tab, invalid WindowID"))
		return INDEX_NONE;
	}

	const int32 NewTabID = GenerateNewTabID();

	TabsData.Add(NewTabID, TabData);
	WindowsData[WindowID].Tabs.Add(NewTabID);

	OnCreateNewTab.Broadcast(WindowID, NewTabID);
	
	Save();
	return NewTabID;
}

void UChatStyleSave::RegisterDeleteTab(int32 TabID)
{
	if (!TabsData.Contains(TabID))
	{
		UE_LOG(LogChatStyle, Warning, TEXT("Delete tab, invalid tab id"))
		return;
	}

	const auto WindowID = GetWindowID(TabID);
	if (!ensure(WindowsData.Contains(WindowID))) return;

	WindowsData[WindowID].Tabs.RemoveSingle(TabID);
	TabsData.Remove(TabID);
	
	//delete window if empty
	if (WindowsData[WindowID].Tabs.IsEmpty())
		RegisterDeleteWindow(WindowID);

	OnDeleteTab.Broadcast(WindowID, TabID);
	
	Save();
}

void UChatStyleSave::RegisterMoveTab(int32 TabID, int32 NewWindowID, int32 NewColumn)
{
	if (!WindowsData.Contains(NewWindowID))
	{
		UE_LOG(LogChatStyle, Warning, TEXT("Move tab, invalid Window id, please add window first"))
		return;
	}

	//Delete tab from current position, add in new
	const auto CurrentWindowID = GetWindowID(TabID);
	if (!ensure(WindowsData.Contains(CurrentWindowID))) return;

	auto& CurrentWindow = WindowsData[CurrentWindowID];
	CurrentWindow.Tabs.RemoveSingle(TabID);
	WindowsData[NewWindowID].Tabs.Insert(TabID, NewColumn);

	//delete previous window if empty
	if (CurrentWindow.Tabs.IsEmpty())
		RegisterDeleteWindow(CurrentWindowID);

	Save();
}

int32 UChatStyleSave::RegisterNewWindow(FChatWindowData WindowData)
{
	const int32 NewWindowID = GenerateNewWindowID();
	WindowsData.Add(NewWindowID, WindowData);
	return NewWindowID;
}

void UChatStyleSave::RegisterDeleteWindow(int32 WindowID)
{
	if (!ensure(WindowsData.Contains(WindowID))) return;
	WindowsData.Remove(WindowID);
}

void UChatStyleSave::RegisterMoveWindow(int32 WindowID, FVector2D NewAbsPosition)
{
	if (!ensure(WindowsData.Contains(WindowID))) return;;
	WindowsData[WindowID].AbsoluteCoordinate = NewAbsPosition;

	Save();
}

void UChatStyleSave::RegisterResizeWindow(int32 WindowID, FVector2D NewAbsPosition, FVector2D NewWindowSize)
{
	if (!ensure(WindowsData.Contains(WindowID))) return;
	auto& WindowData = WindowsData[WindowID];
	WindowData.AbsoluteCoordinate = NewAbsPosition;
	WindowData.Size = NewWindowSize;

	Save();
}

void UChatStyleSave::SetListenChannelsInTab(int32 TabID, const TArray<FGameplayTag>& NewListenChannels)
{
	if (!TabsData.Contains(TabID)) return;

	TabsData[TabID].ListenChannels.Empty();
	TabsData[TabID].ListenChannels.Append(NewListenChannels);
}

bool UChatStyleSave::IsListenChannel(int32 TabID, FGameplayTag Channel) const
{
	if (!ensure(TabsData.Contains(TabID))) return false;
	return TabsData[TabID].ListenChannels.IsEmpty() || TabsData[TabID].ListenChannels.Contains(Channel);
}


const FChatWindowData& UChatStyleSave::GetWindowData(int32 WindowID) const
{
	if (WindowsData.Contains(WindowID))
		return WindowsData[WindowID];

	ensure(0);
	static FChatWindowData EmptyWindowData;
	return EmptyWindowData;
}

const FChatTabData& UChatStyleSave::GetTabData(int32 TabID) const
{
	if (TabsData.Contains(TabID))
		return TabsData[TabID];

	static FChatTabData DefaultTabData;
	return DefaultTabData;
}

void UChatStyleSave::GetPlayerCanWriteChannels(TArray<FGameplayTag>& OutChannels) const
{
	if (!ensure(DefaultChannelsInfo)) return;
	DefaultChannelsInfo->GetPlayerCanWriteChannels(OutChannels);
}

const FChatChannelInfo& UChatStyleSave::GetChannelDefaultInfo(FGameplayTag Channel) const
{
	if (DefaultChannelsInfo)
	{
		return DefaultChannelsInfo->GetChannelInfo(Channel);
	}

	UE_LOG(LogChatStyle, Error, TEXT("Get default channel info for invalid channel %s, add this channel to Default data"), *Channel.ToString())
	static FChatChannelInfo EmptyChannelInfo;
	return EmptyChannelInfo;
}

int32 UChatStyleSave::GetWindowID(int32 TabID) const
{
	for (const auto& [WindowID, WindowData] : WindowsData)
	{
		if (WindowData.Tabs.Contains(TabID)) return WindowID;
	}

	UE_LOG(LogChatStyle, Error, TEXT("Window for tab with id %d not found"), TabID)
	return INDEX_NONE;
}

void UChatStyleSave::ResetTabStyle(int32 TabID, bool bResetWindowSettings)
{
	UE_LOG(LogChatStyle, Display, TEXT("Reset tab style, TabId = %d"), TabID)
	if (TabsData.Contains(TabID))
	{
		TabsData[TabID].ChannelsStyles.Empty();

		const auto WindowID = GetWindowID(TabID);
		if (bResetWindowSettings)
		{
			ensure(WindowsData.Contains(WindowID));

			auto& Window = WindowsData[WindowID];
			const auto Default = FChatWindowData();

			Window.RenderOpacity = Default.RenderOpacity;
		}

		Apply(WindowID);
	}
	else
		UE_LOG(LogChatStyle, Warning, TEXT("Unknown TabID"))
}

void UChatStyleSave::SetWindowDataFrom(int32 WindowID, FChatWindowData WindowData)
{
	WindowsData.FindOrAdd(WindowID) = WindowData;
}

void UChatStyleSave::SetTabDataFrom(int32 TabID, FChatTabData TabData)
{
	TabsData.FindOrAdd(TabID) = TabData;
}

void UChatStyleSave::SetChannelStyle(int32 TabID, FGameplayTag Channel, FChatChannelStyle NewStyle)
{
	if (!ensure(TabsData.Contains(TabID))) return;

	auto& StyleOverrides = TabsData[TabID].ChannelsStyles;
	StyleOverrides.Add(Channel, NewStyle);

	//notify update
	OnWindowStyleUpdate.Broadcast(GetWindowID(TabID));
}

FLinearColor UChatStyleSave::GetMessageColor(int32 TabID, FGameplayTag Channel) const
{
	if (TabsData.Contains(TabID))
	{
		if (TabsData[TabID].ChannelsStyles.Contains(Channel))
			return TabsData[TabID].ChannelsStyles[Channel].TextColor;
	}

	if (!ensure(DefaultChannelsInfo)) return FLinearColor::White;
	return DefaultChannelsInfo->GetChannelInfo(Channel).UIColor;
}

int32 UChatStyleSave::GetFontSize(int32 TabID, FGameplayTag Channel) const
{
	if (!ensure(TabsData.Contains(TabID)))
	{
		return 12;
	}

	const auto& Tab = TabsData[TabID];
	const auto ChannelStyle = Tab.ChannelsStyles.Find(Channel);

	return Tab.bUseGeneralFontSize || !ChannelStyle
		? Tab.GeneralFontSize
		: ChannelStyle->FontSize;
}

int32 UChatStyleSave::GetTabDefaultTextSize(int32 TabID) const
{
	return GetTabData(TabID).GeneralFontSize;
}

void UChatStyleSave::SetTabDefaultTextSize(int32 TabID, int32 InSize, bool bUseDefaultSize)
{
	if (!ensure(TabsData.Contains(TabID))) return;

	TabsData[TabID].GeneralFontSize = InSize;
	TabsData[TabID].bUseGeneralFontSize = bUseDefaultSize;

	//notify update
	OnWindowStyleUpdate.Broadcast(GetWindowID(TabID));
}

void UChatStyleSave::SetShowWindowOnMessage(int32 WindowID, bool bShow)
{
	if (!ensure(WindowsData.Contains(WindowID))) return;;
	WindowsData[WindowID].bShowWindowOnMessage = bShow;

	//notify update
	OnWindowStyleUpdate.Broadcast(WindowID);
}

bool UChatStyleSave::IsShowWindowOnMessage(int32 WindowID) const
{
	ensure(WindowsData.Contains(WindowID));
	return WindowsData.Contains(WindowID) ? WindowsData[WindowID].bShowWindowOnMessage : false; 
}

void UChatStyleSave::SetShowMessageTime(int32 TabID, bool bShowTime)
{
	if (!ensure(TabsData.Contains(TabID))) return;;
	TabsData[TabID].bShowTime = bShowTime;

	//notify update
	OnWindowStyleUpdate.Broadcast(GetWindowID(TabID));
}

void UChatStyleSave::SetTabName(int32 TabID, const FText& Name)
{
	if (!ensure(TabsData.Contains(TabID))) return;;
	TabsData[TabID].TabName = Name;	

	//notify update
	OnWindowStyleUpdate.Broadcast(GetWindowID(TabID));
}

void UChatStyleSave::SetShowMessagesCount(int32 WindowID, bool bShow)
{
	if (!ensure(WindowsData.Contains(WindowID))) return;
	WindowsData[WindowID].bShowNewMessagesCount = bShow;

	//notify update
	OnWindowStyleUpdate.Broadcast(WindowID);
}

void UChatStyleSave::SetWindowOpacity(int32 WindowID, float Opacity)
{
	if (!ensure(WindowsData.Contains(WindowID))) return;
	WindowsData[WindowID].RenderOpacity = Opacity;

	//notify update
	OnWindowStyleUpdate.Broadcast(WindowID);
}

int32 UChatStyleSave::GenerateNewTabID() const
{
	for (int32 NewID = 2; ; ++NewID)
	{
		if (!TabsData.Contains(NewID)) return NewID;
	}
}

int32 UChatStyleSave::GenerateNewWindowID() const
{
	for (int32 NewID = 2; ; ++NewID)
	{
		if (!WindowsData.Contains(NewID)) return NewID;
	}
}
