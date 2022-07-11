// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Settings/NSGameSetting.h"
#include "NSSettingsUIInfo.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class NETWORKSHOTER_API UNSSettingsUIInfo : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere)
	TSubclassOf<UNSSettingUIInfo> AntiAliasing;
	
	UPROPERTY(config, EditAnywhere)
	TSubclassOf<UNSSettingUIInfo> Shadows;
	
	UPROPERTY(config, EditAnywhere)
	TSubclassOf<UNSSettingUIInfo> PostProcess;

	static UNSSettingsUIInfo* Get();
};
