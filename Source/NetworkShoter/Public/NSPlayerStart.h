// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "NSPlayerStart.generated.h"

/**
 * PlayerStart with division by commands
 */
UCLASS()
class NETWORKSHOTER_API ANSPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:	
	/** Return true if pawn can be spawned in this point
	 */
	bool CanSpawn(AController* Controller);
protected:
	/** indexes of teams who can be spawned here */
	UPROPERTY(EditAnywhere)
	TArray<int32> TeamIndexes;
};
