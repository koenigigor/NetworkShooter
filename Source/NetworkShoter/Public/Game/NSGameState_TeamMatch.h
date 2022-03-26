// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/NSGameState.h"
#include "NSGameState_TeamMatch.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState_TeamMatch : public ANSGameState
{
	GENERATED_BODY()
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
