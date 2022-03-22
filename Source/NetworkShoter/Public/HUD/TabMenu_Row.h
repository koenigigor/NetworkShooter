// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "TabMenu_Row.generated.h"

class ANSPlayerState;

/**
 * widget for player statistic
 * must be stored in widget "TabMenu"
 */
UCLASS()
class NETWORKSHOTER_API UTabMenu_Row : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(ANSPlayerState* PlayerState = nullptr);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnStatisticUpdate();

protected:
	UPROPERTY(BlueprintReadOnly)
	ANSPlayerState* OwningPlayerState = nullptr;

	//called when player who response for this row spawn/death
	UFUNCTION()
	void OnPawnDeath(ANSPlayerState* PlayerState);
	UFUNCTION()
	void OnPawnSpawn(ANSPlayerState* PlayerState);

	UFUNCTION(BlueprintPure)
	FPlayerStatistic GetPlayerStatistic();

	UFUNCTION(BlueprintPure)
	FString GetPlayerName();

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;
};
