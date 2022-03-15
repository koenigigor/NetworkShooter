// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "PCNetShooter.generated.h"

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
};
