// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "NSGameUserSettings.generated.h"

class UNSGameSetting;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsUpdateDelegate);

/* enum describe settings, for iterate in map, (for easy set it into ui) */
UENUM()
enum class ENSSetting : uint8
{
	None,
	AntiAliasing,
	Texture,
	GlobalIllumination,
	Shadows,
	PostProcess
	
};

/** Setting class with model setting */
UCLASS()
class NETWORKSHOTER_API UNSGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
public:
	UNSGameUserSettings();
	
	static UNSGameUserSettings* Get();

	TMap<ENSSetting, UNSGameSetting*>& GetVideoSettings(){ return VideoSettings; }

	void SetRecommend();

	UPROPERTY(BlueprintAssignable)
	FSettingsUpdateDelegate OnSettingsUpdate;

protected:
	UPROPERTY()
	TMap<ENSSetting, UNSGameSetting*> VideoSettings;
};
