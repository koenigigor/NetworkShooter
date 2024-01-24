// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameUserSettings.h"
#include "Blueprint/UserWidget.h"
#include "Settings/NSGameSetting.h"
#include "WSettingOption.generated.h"

class UTextBlock;
class UButton;
class UComboBoxString;
class USlider;

/** Single setting option widget */
UCLASS()
class NETWORKSHOTER_API UWSettingOption : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	
public:
	void Init(UNSGameSetting* InSetting);
	void UpdateText();

	UPROPERTY(EditAnywhere, Category = "Setup")
	ENSSetting TargetSetting; 

protected:
	UFUNCTION()
	void OnNextOption();

	UFUNCTION()
	void OnPreviousOption();
	
	UFUNCTION()
	void OnSliderValueChanged(float Value);
	
	UFUNCTION()
	void OnOptionSelect(FString SelectedItem, ESelectInfo::Type SelectionType);
	
protected:
	TWeakObjectPtr<UNSGameSetting> Setting;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SettingName = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SettingOption = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* PreviousOptionButton = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* NextOptionButton = nullptr;

	UPROPERTY(meta=(BindWidget, OptionalWidget=true))
	UComboBoxString* OptionsBox = nullptr;

	UPROPERTY(meta=(BindWidget))
	USlider* OptionSlider = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* OptionSliderValue = nullptr;
};
