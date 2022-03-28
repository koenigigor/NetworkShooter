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
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_Refresh();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWConnectedPlayer> WConnectedPlayerClass = nullptr;

	UFUNCTION()
	void PlayerChanged(APlayerState* Player);
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UWConnectedPlayer*> PlayerWidgetsArray;

	bool bConstructed = false;
};
