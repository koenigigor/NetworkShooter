// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "NSSpectator.generated.h"

UENUM()
enum class ESpectatorMode : uint8
{
	Free,
	AttachToActor,
	AroundActor
};

/**
 * Default spectator pawn class for network shooter
 * TODO Client only
 */

UCLASS()
class NETWORKSHOTER_API ANSSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANSSpectator();

	UFUNCTION(BlueprintPure)
	ESpectatorMode GetSpectatorMode() const { return SpectatorMode; };
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//if mode Attached, swap actor to next
	UFUNCTION(BlueprintCallable)
	void SwapAttachedActor(bool bNext=true);
	
	//Update AttachedActor reference
	void UpdateAttachedActor(bool bNext=true);
	
	//Set spectator mode
	UFUNCTION(BlueprintCallable)
	void SetSpectatorMode(ESpectatorMode Mode);

	/** can fly arround all map */
	void SetModeFree();

	/** View from other actor camera */
	void SetModeAttachToActor();

	/** Attach to other actor, can rotate camera */
	void SetModeAroundActor();

	/*----------------------------*/
	/** Exit from spectator mode **/
	void ExitModeAttachToActor();
	void ExitModeAroundActor();

	UPROPERTY(VisibleAnywhere)
	ESpectatorMode SpectatorMode = ESpectatorMode::Free;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Actor who we attach (in AttachTo mode) */
	TPair<AActor*, int32> AttachedActor;

	UFUNCTION(BlueprintCallable)
	AActor* GetAttachedActor();
};
