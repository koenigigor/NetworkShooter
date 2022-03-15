// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameMode.h"
#include "GMDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API AGMDeathMatch : public ANSGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Deathmatch")
	float RespawnDelay = 5.f;
	
	
	virtual void CharacterKilled(APawn* WhoKilled, AController* InstigatedBy, AActor* DamageCauser) override;

	void RespawnDeathPlayer();
};
