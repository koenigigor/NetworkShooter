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
