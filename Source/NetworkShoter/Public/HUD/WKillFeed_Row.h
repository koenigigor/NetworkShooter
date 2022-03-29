// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/NSGameState.h"
#include "WKillFeed_Row.generated.h"


/**
 * Kill Feed Row, support widget for WKillFeed
 * keep row info about kill
 */
UCLASS()
class NETWORKSHOTER_API UWKillFeed_Row : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(FDamageInfo KillInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Init(FDamageInfo KillInfo);
};
