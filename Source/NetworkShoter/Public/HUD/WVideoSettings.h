// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WVideoSettings.generated.h"

class UVerticalBox;
class UWSettingOption;
class UButton;

/** Video settings options widget */
UCLASS()
class NETWORKSHOTER_API UWVideoSettings : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UWSettingOption> SettingOptionClass = nullptr;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* OptionsBox = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* SetRecommendOptionsButton = nullptr;

	UFUNCTION()
	void OnSettingsUpdate();
	
protected:
	UFUNCTION()
    void SetRecommendOptions();
};
