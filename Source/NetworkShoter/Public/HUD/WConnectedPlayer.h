// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WConnectedPlayer.generated.h"

class APlayerState;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UWConnectedPlayer : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Refresh(APlayerState* Player);
};
