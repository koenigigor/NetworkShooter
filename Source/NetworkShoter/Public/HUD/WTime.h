// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WTime.generated.h"

class ANSGameState;

/**
 * Base widget for show match time
 */
UCLASS()
class NETWORKSHOTER_API UWTime : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;

	float GetMatchTime();

	UFUNCTION(BlueprintCallable)
	void GetReadableMatchTime(int32& Minutes, int32& Seconds);

	/** if match in progress return time end percentage */
	UFUNCTION(BlueprintPure)
	float GetMatchTimeProgress();
	
	ANSGameState* GameState = nullptr;
};
