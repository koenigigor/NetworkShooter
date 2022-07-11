// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "NSGameUserSettings.generated.h"

class UNSGameSetting;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsUpdateDelegate);

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
public:
	UNSGameUserSettings();
	
	static UNSGameUserSettings* Get();

	TArray<UNSGameSetting*>& GetVideoSettings(){ return VideoSettings; }

	void SetRecommend();

	UPROPERTY(BlueprintAssignable)
	FSettingsUpdateDelegate OnSettingsUpdate;

protected:
	UPROPERTY()
	TArray<UNSGameSetting*> VideoSettings;
};
