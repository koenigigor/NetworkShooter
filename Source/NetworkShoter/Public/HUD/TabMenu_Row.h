// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/NSPlayerState.h"
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
	virtual void NativeConstruct() override;
	
	void Init(ANSPlayerState* PlayerState = nullptr);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnStatisticUpdate();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCharacterDead();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCharacterRespawn();

protected:
	UPROPERTY(BlueprintReadOnly)
	ANSPlayerState* OwningPlayerState = nullptr;

	//called when player who response for this row spawn/death
	UFUNCTION()
	void OnCharacterDeath(APawn* DeadPawn);

	UFUNCTION(BlueprintPure)
	FPlayerStatistic GetPlayerStatistic();

	UFUNCTION(BlueprintPure)
	FString GetPlayerName();

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;
};
