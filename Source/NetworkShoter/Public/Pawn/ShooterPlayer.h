// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Input/NSInputAction.h"
#include "ShooterPlayer.generated.h"

class UNSAbilitySystemComponent;
class UNetShooterAttributeSet;
class UWeaponAttributeSet;
class UItemAbilitiesData;

USTRUCT()
struct FInputActions
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Jump = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Crouch = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Move = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Rotation = nullptr;

	// confirm ability
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Confirm = nullptr;

	// cancel ability 
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Cancel = nullptr;
};

USTRUCT()
struct FInputMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UInputMappingContext* InputMappingContext = nullptr;
	
	UPROPERTY(EditAnywhere)
	FInputActions InputActions;

	UPROPERTY(EditDefaultsOnly)
	TArray<UNSInputAction*> AbilityInputs;
};

UCLASS()
class NETWORKSHOTER_API AShooterPlayer : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AShooterPlayer();

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void PawnClientRestart() override;
	
	//~==============================================================================================
	// Ability System
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	void GiveStartupAbilities();

	// Attribute delegates
protected:
	void BindAttributeDelegates();
	
	void OnHealthChange(const FOnAttributeChangeData& Data);
	
	UFUNCTION(BlueprintImplementableEvent)
	void HealthChanged(float OldValue);
	
	void OnArmorChange(const FOnAttributeChangeData& Data);

	void OnWalkSpeedChange(const FOnAttributeChangeData& Data);
	
	UFUNCTION(BlueprintImplementableEvent)
	void WalkSpeedChanged(float OldValue);

	
public:
	/** [Multicast] Called from death gameplay ability, when player Die */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Death();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_CharacterDead();

	virtual FGenericTeamId GetGenericTeamId() const override;
	
	virtual FVector GetPawnViewLocation() const override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InputTagPress(FGameplayTag Tag);
	void InputTagRelease(FGameplayTag Tag);

	void MovementInput(const FInputActionValue& InputActionValue);
	void InputRotation(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNSAbilitySystemComponent* AbilitySystem;
	
	UPROPERTY(BlueprintReadOnly)
	UNetShooterAttributeSet* CharacterAttributeSet;

	UPROPERTY(BlueprintReadOnly)
	UWeaponAttributeSet* WeaponAttributeSet;

	/** Abilities register on begin play and binds with input actions */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	/** All item abilities info (see UItemAbilitiesData description) */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UItemAbilitiesData* ItemAbilities = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FInputMapping InputMapping;
};
