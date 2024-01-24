// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/NSGameUserSettings.h"

#include "Settings/NSGameSetting.h"

DEFINE_LOG_CATEGORY_STATIC(LogNSUserSettings, All, All);

#define LOCTEXT_NAMESPACE "GameUserSettings"

UNSGameUserSettings::UNSGameUserSettings()
{
	const TArray<FSettingOption> VideoOptions =
	{
		{LOCTEXT("LowSetting", "Low"), 0},
		{LOCTEXT("MediumSetting", "Medium"), 1},
		{LOCTEXT("HighSetting", "High"), 2},
		{LOCTEXT("UltraSettings", "Fire"), 3}
	};

	VideoSettings =
	{{ENSSetting::AntiAliasing,
	  UNSGameSetting::NewSetting(
		  LOCTEXT("AntiAliasingSettingName", "Anti-Aliasing"),
		  VideoOptions,
		  [&]() { return GetAntiAliasingQuality(); },
		  [&](int32 NewValue)
		  {
			  SetAntiAliasingQuality(NewValue);
			  ApplySettings(false);
		  })},
	 {ENSSetting::Texture,
	  UNSGameSetting::NewSetting(
		  LOCTEXT("TexturesSettingName", "Textures"),
		  VideoOptions,
		  [&]() { return GetTextureQuality(); },
		  [&](int32 NewValue)
		  {
			  SetTextureQuality(NewValue);
			  ApplySettings(false);
		  })},
	 {ENSSetting::GlobalIllumination,
	  UNSGameSetting::NewSetting(
		  LOCTEXT("GlobalIlluminationSettingName", "Global Illumination"),
		  VideoOptions,
		  [&]() { return GetGlobalIlluminationQuality(); },
		  [&](int32 NewValue)
		  {
			  SetGlobalIlluminationQuality(NewValue);
			  ApplySettings(false);
		  })},
	 {ENSSetting::Shadows,
	  UNSGameSetting::NewSetting(
		  LOCTEXT("ShadowsSettingName", "Shadows"),
		  VideoOptions,
		  [&]() { return GetShadowQuality(); },
		  [&](int32 NewValue)
		  {
			  SetShadowQuality(NewValue);
			  ApplySettings(false);
		  })},
	 {ENSSetting::PostProcess,
	  UNSGameSetting::NewSetting(
		  LOCTEXT("PostProcessingSettingName", "PostProcessing"),
		  VideoOptions,
		  [&]() { return GetPostProcessingQuality(); },
		  [&](int32 NewValue)
		  {
			  SetPostProcessingQuality(NewValue);
			  ApplySettings(false);
		  })}
	};
}

UNSGameUserSettings* UNSGameUserSettings::Get()
{
	const auto Settings = GEngine ? Cast<UNSGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;

	if (!Settings)
	{
		UE_LOG(LogNSUserSettings, Error, TEXT("UNSGameUserSettings not set as default settings class"))
	}

	return Settings;
}

void UNSGameUserSettings::SetRecommend()
{
	RunHardwareBenchmark();
	ApplySettings(false);
	OnSettingsUpdate.Broadcast();
}

#undef LOCTEXT_NAMESPACE
