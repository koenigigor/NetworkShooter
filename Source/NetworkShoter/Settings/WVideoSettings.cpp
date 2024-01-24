// Fill out your copyright notice in the Description page of Project Settings.


#include "WVideoSettings.h"

#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Settings/NSGameUserSettings.h"
#include "WSettingOption.h"

void UWVideoSettings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(OptionsBox);
	
	const auto UserSettings = UNSGameUserSettings::Get();
	if (!UserSettings) return;
    
	UserSettings->LoadSettings();
	UserSettings->OnSettingsUpdate.AddDynamic(this, &ThisClass::UWVideoSettings::OnSettingsUpdate);

	check(SetRecommendOptionsButton);
	SetRecommendOptionsButton->OnClicked.AddDynamic(this, &ThisClass::SetRecommendOptions);
}

void UWVideoSettings::OnSettingsUpdate()
{
	for (const auto Child : OptionsBox->GetAllChildren())
	{
		if (const auto Setting = Cast<UWSettingOption>(Child))
		{
			Setting->UpdateText();
		}
	} 
}

void UWVideoSettings::SetRecommendOptions()
{
	const auto UserSettings = UNSGameUserSettings::Get();
	if (!UserSettings) return;

	UserSettings->SetRecommend();
}
