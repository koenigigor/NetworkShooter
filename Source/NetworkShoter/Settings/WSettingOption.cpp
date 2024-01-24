// Fill out your copyright notice in the Description page of Project Settings.


#include "WSettingOption.h"

#include "NSGameUserSettings.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"

void UWSettingOption::NativePreConstruct()
{
	Super::NativePreConstruct();

    if (const auto Settings = UNSGameUserSettings::Get())
    {
		Init(Settings->GetVideoSettings().Contains(TargetSetting) ? Settings->GetVideoSettings()[TargetSetting] : nullptr);
    }
}

void UWSettingOption::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(PreviousOptionButton);
	check(NextOptionButton);

	PreviousOptionButton->OnClicked.AddDynamic(this, &ThisClass::OnPreviousOption);
	NextOptionButton->OnClicked.AddDynamic(this, &ThisClass::UWSettingOption::OnNextOption);

	if (OptionSlider)
	{
		OptionSlider->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderValueChanged);
	}

	if (OptionsBox)
	{
		OptionsBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnOptionSelect);
	}
}

void UWSettingOption::Init(UNSGameSetting* InSetting)
{
	if (!InSetting) return;
	
	Setting = MakeWeakObjectPtr(InSetting);

	OptionSlider->SetMinValue(Setting->GetMinValue());
	OptionSlider->SetMaxValue(Setting->GetMaxValue());
	
	UpdateText();
}

void UWSettingOption::UpdateText()
{
	if (Setting.IsValid())
	{
		SettingName->SetText(Setting->GetName());
		SettingOption->SetText(Setting->GetCurrentOption().Name);

		OptionsBox->ClearOptions();
		for (const auto Option : Setting->GetOptions())
			OptionsBox->AddOption(Option.Name.ToString());
		
		OptionsBox->SetSelectedOption(Setting->GetCurrentOption().Name.ToString());

		OptionSlider->SetValue(Setting->GetCurrentValue());

		FNumberFormattingOptions Options;
		Options.MinimumFractionalDigits=0;
		Options.MaximumFractionalDigits=0;
		OptionSliderValue->SetText(FText::AsNumber(Setting->GetCurrentValue(), &Options));
	}
}

void UWSettingOption::OnSliderValueChanged(float Value)
{
	if (!Setting.IsValid()) return;

	Setting->SetValue(Value);
	UpdateText();
}

void UWSettingOption::OnOptionSelect(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::OnMouseClick)
	{
		const auto OptionToSelect = Setting->GetOptions().FindByPredicate([&](const auto& Option)
		{
			return Option.Name.ToString() == SelectedItem;
		});

		if (OptionToSelect)
		{
			Setting->SetValue(OptionToSelect->Value);
			UpdateText();
		}
	}
}

void UWSettingOption::OnNextOption()
{
	if (!Setting.IsValid()) return;

	Setting.Get()->ApplyNextOption();
	UpdateText();
}

void UWSettingOption::OnPreviousOption()
{
	if (!Setting.IsValid()) return;

	Setting.Get()->ApplyPreviousOption();
	UpdateText();
}
