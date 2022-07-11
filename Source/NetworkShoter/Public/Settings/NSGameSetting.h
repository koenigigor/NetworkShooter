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
	//setting visible name (eq "Low", "Medium")
	FText Name; 
	
	int32 Value;

	static FSettingOption GetErrorOption();
	
	bool operator<(const FSettingOption& Other) const { return Value < Other.Value; };
};

//preferred setting ui type
UENUM(BlueprintType)
enum class ESettingUIType : uint8
{
	ButtonUpDown,	// |-|option|+|
	Slider,			// |val|----*-|
	Select			// |  option^ |
};

UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Blueprintable)
class UNSSettingUIInfo : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESettingUIType UIType;

	///Can add here other stuff
	///like tip-tool,
	///setting image (speaker for Volume setting)
};


/** Describe one setting and his options \n
 *	Like:
 *	- Name: Texture Quality
 *	- Options: Low, Medium, High
 */
UCLASS()
class UNSGameSetting : public UObject
{
	GENERATED_BODY()
public:
	static UNSGameSetting* NewSetting(FText InName, TArray<FSettingOption> InOptions, TFunction<int32()> InGetter, TFunction<void(int32)> InSetter, TSubclassOf<UNSSettingUIInfo> InUIInfo = nullptr);

	FSettingOption GetCurrentOption();
	FText GetName() { return Name; }
	TArray<FSettingOption>& GetOptions() { return Options; }

	int32 GetMinValue();
	int32 GetMaxValue();
	int32 GetCurrentValue() const;
	ESettingUIType GetUISettingType() const { return UIInfo ? GetDefault<UNSSettingUIInfo>(UIInfo)->UIType : ESettingUIType::ButtonUpDown; };

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

public:
	TSubclassOf<UNSSettingUIInfo> UIInfo;
};
