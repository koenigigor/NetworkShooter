// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WConnectedPlayers.generated.h"

class UWConnectedPlayer;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UWConnectedPlayers : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	/** Trigger when PlayerWidgetsArray updated */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_Refresh();

	/** Trigger when new player connect/disconnect to game */
	UFUNCTION()
	void PlayerChanged(APlayerState* Player);

	
	/** Widget with name connected player */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UWConnectedPlayer> WConnectedPlayerClass = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UWConnectedPlayer*> PlayerWidgetsArray;

	/** call NativeConstruct one time */
	bool bConstructed = false;
};
