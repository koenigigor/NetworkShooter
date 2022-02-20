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
	// Sets default values for this character's properties
	AShooterPlayer();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystem;
	
	UPROPERTY(BlueprintReadOnly)
	UNetShooterAttributeSet* CharacterAttributeSet;

	UPROPERTY(BlueprintReadOnly)
	UWeaponAttributeSet* WeaponAttributeSet;

	/** Abilities register on begin ply and binds with input actions */
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UFUNCTION(BlueprintCallable)
	void AddAbility(TSubclassOf<UGameplayAbility> Ability);

///Attribute delegates
	void BindAttributeDelegates();

	//UFUNCTION() //not work with FOnAttributeChangeData
	void OnHealthChange(const FOnAttributeChangeData& Data);
	
	UFUNCTION(BlueprintImplementableEvent)
	void HealthChanged(float OldValue);
	
	//UFUNCTION()
	void OnArmorChange(const FOnAttributeChangeData& Data);
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
