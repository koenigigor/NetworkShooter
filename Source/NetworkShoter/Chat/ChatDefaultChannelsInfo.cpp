// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/ChatDefaultChannelsInfo.h"

#include "NSStructures.h"

#define LOCTEXT_NAMESPACE "GameUserSettings"

UChatDefaultChannelsInfo::UChatDefaultChannelsInfo()
{
	//all chat categories
	ChannelGroups =
	{
		{
			LOCTEXT("GeneralChatChannel", "Chat"),
			{
					{
						TAG_CHAT_GENERAL,
						FChatChannelInfo(
							FLinearColor::White,
							LOCTEXT("GeneralChatChannel", "General"),
							true)},
					{
						TAG_CHAT_GROUP,
						FChatChannelInfo(
							FLinearColor::Blue,
							LOCTEXT("GroupChatChannel", "Group"),
							true)}
			}
		},
		{
			LOCTEXT("GeneralChatChannel", "Battle"),
			{
				//Damage, Heal ...
			}
		}
	};
}

#undef LOCTEXT_NAMESPACE


const FChatChannelInfo& UChatDefaultChannelsInfo::GetChannelInfo(FGameplayTag Channel) const
{
	for (const auto& Group : ChannelGroups)
	{
		if (Group.Channels.Contains(Channel))
		{
			return Group.Channels[Channel];
		}
	}

	ensure(false);
	static FChatChannelInfo InvalidInfo(FLinearColor::Gray, FText::FromString("InvalidChannel"), false);
	return InvalidInfo;
}

void UChatDefaultChannelsInfo::GetPlayerCanWriteChannels(TArray<FGameplayTag>& OutChannels) const
{
	OutChannels.Empty();
	
	for (const auto& Group : ChannelGroups)
	{
		for (const auto& [Channel, Info] : Group.Channels)
		{
			if (Info.bPlayerCanWrite) OutChannels.Add(Channel);
		}
	}
}
