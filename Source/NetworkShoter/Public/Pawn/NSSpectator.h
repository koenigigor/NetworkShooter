// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "NSSpectator.generated.h"

class ANSPlayerState;

UENUM()
enum class ESpectatorMode : uint8
{
	None,
	Free,
	AttachToActor,
	AroundActor
};

/**
 * Default spectator pawn class for network shooter
 * Client only
 */

UCLASS()
class NETWORKSHOTER_API ANSSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ANSSpectator();
	
	/* if mode Attached, swap actor to next */
	UFUNCTION(BlueprintCallable)
	void ChangeAttachedActor(bool bNext=true, bool bOnlyLive=true);

	/* Set spectator mode */
    UFUNCTION(BlueprintCallable)
    void SetSpectatorMode(ESpectatorMode Mode);

	UFUNCTION(BlueprintPure)
	ANSPlayerState* GetAttachedPlayer() const { return CurrentAttachedPlayer; };

	UFUNCTION(BlueprintPure)
	ESpectatorMode GetSpectatorMode() const { return SpectatorMode; };

	
protected:
	/** can fly freely */
	void SetModeFree();
	void EndModeFree();
    	
	/** View from other actor camera */
	void SetModeAttachToActor();
	void EndModeAttachToActor();
    
	/** Attach to other actor, can rotate camera */
	void SetModeAroundActor();
	void EndModeAroundActor();
	
protected:
	/** Update CurrentAttachedPlayer */
	void GetNextPlayerToAttach(bool bNext=true, bool bOnlyLive=true);

	APawn* GetPawnToAttach(); 

private:
	UPROPERTY(VisibleAnywhere)
	ESpectatorMode SpectatorMode = ESpectatorMode::Free;
	
	/** Player who we attach (in AttachTo mode) */
	UPROPERTY()
	ANSPlayerState* CurrentAttachedPlayer = nullptr;
};
