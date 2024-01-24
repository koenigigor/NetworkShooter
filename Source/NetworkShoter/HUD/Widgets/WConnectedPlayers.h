// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WConnectedPlayers.generated.h"

class UWConnectedPlayer;
class UVerticalBox;

/** Single row of WConnectedPlayers */
UCLASS(Abstract)
class NETWORKSHOTER_API UWConnectedPlayer : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Refresh(APlayerState* Player);
};

/** Table with Connected players */
UCLASS(Abstract)
class NETWORKSHOTER_API UWConnectedPlayers : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;

	/** Trigger when new player connect/disconnect to game */
	UFUNCTION()
	void OnPlayersCountChange(APlayerState* Player);

	
	/** Widget with name connected player */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UWConnectedPlayer> WConnectedPlayerClass = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UWConnectedPlayer*> PlayerWidgetsArray;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* PlayersBox = nullptr;
};
