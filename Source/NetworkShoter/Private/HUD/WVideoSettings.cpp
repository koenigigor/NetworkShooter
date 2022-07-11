// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WVideoSettings.h"

#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Settings/NSGameUserSettings.h"
#include "HUD/WSettingOption.h"

void UWVideoSettings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(OptionsBox);
	check(SettingOptionClass);

	OptionsBox->ClearChildren();
	const auto UserSettings = UNSGameUserSettings::Get();
	if (!UserSettings) return;
    
	UserSettings->LoadSettings();
	UserSettings->OnSettingsUpdate.AddDynamic(this, &ThisClass::UWVideoSettings::OnSettingsUpdate);
    	
	for (const auto& Setting : UserSettings->GetVideoSettings())
	{
		const auto SettingOption = CreateWidget<UWSettingOption>(GetOwningPlayer(), SettingOptionClass);
		SettingOption->Init(Setting);
		OptionsBox->AddChildToVerticalBox(SettingOption);
	}

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
