// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamAttitude.h"
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
	//ANSPlayerStart();
	
	/** Return true if pawn can be spawned in this point
	 */
	bool CanSpawn(AController* Controller);
protected:
	/** Teams who can be spawned in this point */
	UPROPERTY(EditAnywhere)  //todo , meta=(NoElementDuplicate))
	TArray<EGameTeam> AllowedTeams {EGameTeam::Neutral};
};
