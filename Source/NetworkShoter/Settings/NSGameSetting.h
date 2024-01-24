// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameSetting.generated.h"

/** Pair describe one option of setting. \n
 *	Like:
 *	- Low = 0
 *	- Medium = 1
 */
struct FSettingOption
{
	static FSettingOption ErrorOption;
	
	//setting visible name (eq "Low", "Medium")
	FText Name; 
	
	int32 Value;
	
	bool operator<(const FSettingOption& Other) const { return Value < Other.Value; };
};


/** Model class for setting and his options \n
 *	Like:
 *	- Name: Texture Quality
 *	- Options: Low, Medium, High
 */
UCLASS()
class UNSGameSetting : public UObject
{
	GENERATED_BODY()
public:
	static UNSGameSetting* NewSetting(FText InName, TArray<FSettingOption> InOptions, TFunction<int32()> InGetter, TFunction<void(int32)> InSetter);

	FSettingOption GetCurrentOption();
	FText GetName() { return Name; }
	TArray<FSettingOption>& GetOptions() { return Options; }

	int32 GetMinValue();
	int32 GetMaxValue();
	int32 GetCurrentValue() const;

	void ApplyNextOption();
	void ApplyPreviousOption();
	void SetValue(int32 Value);
	
private:
	int32 GetCurrentIndex();
	
	//Setting name (eq. "Texture quality")
	FText Name;

	TArray<FSettingOption> Options;

	TFunction<int32()> Getter;

	TFunction<void(int32)> Setter;
};
