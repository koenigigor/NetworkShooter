// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ShooterPlayer.generated.h"

class UAbilitySystemComponent;
class UNetShooterAttributeSet;
class UWeaponAttributeSet;

UCLASS()
class NETWORKSHOTER_API AShooterPlayer : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AShooterPlayer();

protected:
	virtual void BeginPlay() override;

	
	//~==============================================================================================
	// Ability System
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	void AddAbility(TSubclassOf<UGameplayAbility> Ability);

	// Attribute delegates
protected:
	void BindAttributeDelegates();
	
	void OnHealthChange(const FOnAttributeChangeData& Data);
	
	UFUNCTION(BlueprintImplementableEvent)
	void HealthChanged(float OldValue);
	
	void OnArmorChange(const FOnAttributeChangeData& Data);

	
public:
	/** [Multicast] Called from death gameplay ability, when player Die */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Death();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_CharacterDead();

	
	virtual FVector GetPawnViewLocation() const override;

	virtual FRotator GetViewRotation() const override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystem;
	
	UPROPERTY(BlueprintReadOnly)
	UNetShooterAttributeSet* CharacterAttributeSet;

	UPROPERTY(BlueprintReadOnly)
	UWeaponAttributeSet* WeaponAttributeSet;

	/** Abilities register on begin play and binds with input actions */
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
