// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCNetShooter.generated.h"

class ANSHUD;
/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API APCNetShooter : public APlayerController
{
	GENERATED_BODY()
public:	
	/** Enter spectator mode */
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	APawn* SpawnSpectator();

	UFUNCTION(BlueprintPure)
	ANSHUD* GetNSHUD() const;

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ClientOnPossess(APawn* InPawn);

	virtual void AcknowledgePossession(APawn* P) override;

	//~==============================================================================================
	// Proxy functions
	
	/** Proxy function for send chat message from chat widget to server */
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SendChatMessage(const FString& Message);

	/** Proxy function for vote for next map (From vote next map widget) */
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void VoteForMap(FName MapRow, bool Up = true);
	
protected:
	//TODO check Spectator be garbage collected on Possess to character

	//virtual bool ShouldKeepCurrentPawnUponSpectating() const override { return true; };
	
private:
	UPROPERTY()
	ANSHUD* NSHUD = nullptr;
};
