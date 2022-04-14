// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "AbilitySystemComponent.h"
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

	/** Notify player about incoming damage for cosmetic (Ui notify, kill feed etc) */
	UFUNCTION(Client, Unreliable, BlueprintCosmetic)
	void NotifyReceiveDamage(float Damage, FVector FromDirection, FName InstigatorName, AActor* DamageCauser);

	/** Enter spectator mode */
	UFUNCTION(BlueprintCosmetic, BlueprintCallable)
	APawn* SpawnSpectator();

	UFUNCTION(BlueprintPure)
	ANSHUD* GetNSHUD();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_Pawn() override;

public:
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
	//UPROPERTY()
	//APawn* Spectator = nullptr;

	
	/* is server side notifiers
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void AmmoChange(float NewAmmo);

	UFUNCTION(BlueprintImplementableEvent)
	void StartReload();

	virtual void OnPossess(APawn* InPawn) override;
public:
	void OnAmmoUpdate(const FOnAttributeChangeData& Data);

	void OnAbilityActivate(UGameplayAbility* ActivatedAbility);
	*/

private:
	ANSHUD* NSHUD = nullptr;
};
