// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NSGameState.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:	
	/** Add pawn in pawn list, called when player possess in Pawn */
	UFUNCTION(BlueprintCallable)
	virtual void AddPlayerPawn(APawn* Pawn, FName Team = "None");

	/** Remove pawn in pawn from list, called when player possess in Pawn */
	UFUNCTION(BlueprintCallable)
	virtual void RemovePawn(APawn* Pawn, FName Team = "None");
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_Team1)
	TArray<APawn*> Team1;
	
	UPROPERTY(ReplicatedUsing=OnRep_Team2)
	TArray<APawn*> Team2;

	UFUNCTION()
	void OnRep_Team1();
	UFUNCTION()
	void OnRep_Team2();
};
