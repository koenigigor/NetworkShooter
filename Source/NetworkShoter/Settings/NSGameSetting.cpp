// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/NSGameSetting.h"

DEFINE_LOG_CATEGORY_STATIC(LogNSGameSetting, All, All);

#define LOCTEXT_NAMESPACE "GameUserSettings"

FSettingOption FSettingOption::ErrorOption({ LOCTEXT("ErrorOption", "ErrorOption"), -1 });

#undef LOCTEXT_NAMESPACE

UNSGameSetting* UNSGameSetting::NewSetting(FText InName, TArray<FSettingOption> InOptions, TFunction<int32()> InGetter, TFunction<void(int32)> InSetter)
{
	const auto Setting = NewObject<UNSGameSetting>();
	
	InOptions.Sort();
	Setting->Name = InName;
	Setting->Options = InOptions;
	Setting->Getter = InGetter;
	Setting->Setter = InSetter;

	return Setting;
}

FSettingOption UNSGameSetting::GetCurrentOption()
{
	const int32 CurrentValue = GetCurrentValue();
		
	//find closest option, because setting can be low ~ 20, medium ~ 40, etc
	
	const auto ClosestBiggerIndex = Options.IndexOfByPredicate([&](const auto& Item)
	{
		return Item.Value >= CurrentValue;
	});

	if (Options.Num() == 0){ return FSettingOption::ErrorOption; }
	if (ClosestBiggerIndex == INDEX_NONE){ return FSettingOption{Options.Last().Name, CurrentValue}; }
	if (ClosestBiggerIndex == 0 || Options[ClosestBiggerIndex].Value == CurrentValue) return Options[ClosestBiggerIndex];
	
	const int32 LeftDelta = CurrentValue - Options[ClosestBiggerIndex-1].Value;
	const int32 RightDelta = Options[ClosestBiggerIndex].Value - CurrentValue;
	const auto ClosestOptionName = (LeftDelta < RightDelta) ? Options[ClosestBiggerIndex-1].Name : Options[ClosestBiggerIndex].Name;
	
	return FSettingOption{ClosestOptionName, CurrentValue};
}

int32 UNSGameSetting::GetMinValue()
{
	return Options.IsValidIndex(0) ? Options[0].Value : 0;
}

int32 UNSGameSetting::GetMaxValue()
{
	return !Options.IsEmpty() ? Options.Last().Value : 0;
}

void UNSGameSetting::ApplyNextOption()
{
	const int32 CurrentIndex = GetCurrentIndex();
	if (CurrentIndex == INDEX_NONE) return;

	const int32 NextIndex = (CurrentIndex + 1) % Options.Num();

	SetValue(Options[NextIndex].Value);
}

void UNSGameSetting::ApplyPreviousOption()
{
	const int32 CurrentIndex = GetCurrentIndex();
	if (CurrentIndex == INDEX_NONE) return;

	const int32 PrevIndex = CurrentIndex == 0 ? Options.Num() - 1 : CurrentIndex - 1;
	SetValue(Options[PrevIndex].Value);
}

void UNSGameSetting::SetValue(int32 Value)
{
	if (!Setter)
	{
		UE_LOG(LogNSGameSetting, Error, TEXT("Setter for setting %s not set"), *Name.ToString())
		return;
	}

	Setter(Value);
}

int32 UNSGameSetting::GetCurrentValue() const
{
	if (!Getter)
	{
		UE_LOG(LogNSGameSetting, Error, TEXT("Getter for setting %s not set"), *Name.ToString())
		return INDEX_NONE;
	}

	return Getter();
}

int32 UNSGameSetting::GetCurrentIndex()
{
	const auto CurrentOptionName = GetCurrentOption().Name;

	return Options.IndexOfByPredicate([&](const auto& Item)
	{
		return Item.Name.IdenticalTo(CurrentOptionName);
	});
}
