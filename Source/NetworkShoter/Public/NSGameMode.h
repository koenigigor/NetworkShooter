// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NSGameMode.generated.h"

/**
 * Base GameMode Class for network shooter
 */
UCLASS()
class NETWORKSHOTER_API ANSGameMode : public AGameMode
{
	GENERATED_BODY()

	/** must be called when character kill someone (other character) */
	void CharacterKilled(APawn* Instigator, AActor* WhoKilled, AActor* Causer);
};
