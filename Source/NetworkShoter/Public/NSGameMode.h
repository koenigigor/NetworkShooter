// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NSGameMode.generated.h"

class ANSPlayerStart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNSPlayerDeath, APawn*, DamagedActor, AController*, DamageInstigator, AActor*, DamageCauser);

/**
 * Base GameMode Class for network shooter
 */
UCLASS()
class NETWORKSHOTER_API ANSGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	/* death characters send this broadcast*/
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FNSPlayerDeath PlayerDeath;

	virtual void BeginPlay() override;

protected:
	/** must be called when character kill someone (other character) */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WhoKilled"))
	void CharacterKilled(APawn* WhoKilled, AController* InstigatedBy, AActor* DamageCauser);

	/** Return spawn points where Pawn can be spawned */
	TArray<ANSPlayerStart*> GetFreePlayerStarts(FName CommandName);

	//Spawn Character in his command player start, and possess to it
	APawn* SpawnPlayer(APlayerController* Controller);

	//Spawn Spectator
	APawn* SpawnSpectator(APlayerController* Controller);
};
