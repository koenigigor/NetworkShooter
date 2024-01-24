// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ChatDefaultChannelsInfo.generated.h"

/** Info about single channel */
USTRUCT(BlueprintType)
struct FChatChannelInfo
{
	GENERATED_BODY()

	FChatChannelInfo() {};

	FChatChannelInfo(FLinearColor InColor, FText InDisplayName, bool InPlayerCanWrite)
		: UIColor(InColor), DisplayName(InDisplayName), bPlayerCanWrite(InPlayerCanWrite) {};

	/** Default color of text in this channel */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor UIColor = FLinearColor::Gray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText DisplayName;

	/** Can Player write in this channel */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPlayerCanWrite = false;
};


/** Info about channels sorted by categories
 *	(Like General = Location, Group, Guild;
 *	System = Notifications, Collectables ... ) */
USTRUCT(BlueprintType)
struct FChannelGroup
{
	GENERATED_BODY()
	
	/** If not use can ser empty,
	 *	Chat / System / Battle ChannelGroup etc. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText GroupName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ForceInlineRow))
	TMap<FGameplayTag, FChatChannelInfo> Channels;
};


/** Default Info about all chat channels, in game */
UCLASS(BlueprintType)
class NETWORKSHOTER_API UChatDefaultChannelsInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UChatDefaultChannelsInfo();
	
	//All default channels data, combined by group
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (TitleProperty = "GroupName"))
	TArray<FChannelGroup> ChannelGroups;

	const FChatChannelInfo& GetChannelInfo(FGameplayTag Channel) const;
	void GetPlayerCanWriteChannels(TArray<FGameplayTag>& OutChannels) const;

};
