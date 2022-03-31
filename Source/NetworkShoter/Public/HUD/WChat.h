// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WChat.generated.h"

class UChatController;
class ANSPlayerState;

/**
 * Widget who response for chat
 */
UCLASS()
class NETWORKSHOTER_API UWChat : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnReceiveMessage(ANSPlayerState* FromPlayer, FString Message);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnReceiveMessage(ANSPlayerState* FromPlayer, const FString& Message);

	UChatController* ChatController = nullptr;
};
