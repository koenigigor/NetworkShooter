// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCNetShooter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLocalPauseDeledate, bool, bOnPause);

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

	/* Show or hide mouse, has count of sources
	 * so if we show 2 times and hide 1 time, mouse still be shoved*/
	UFUNCTION(BlueprintCallable)
	void ShowMouse(bool bShow);

	/* hide mouse and reset counter */
	UFUNCTION(BlueprintCallable)
	void ResetShowMouse();

	/**	client version of pause, disable input and show main menu */
	UFUNCTION(BlueprintCallable)
	void ToggleLocalPause();
	
	UPROPERTY(BlueprintAssignable)
	FLocalPauseDeledate OnLocalPause;
	
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
	
	int8 bMouseIsShoved = 0;

	bool bOnPause = false;
};
