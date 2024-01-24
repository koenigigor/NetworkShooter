// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NetShooterAttributeSet.generated.h"


// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * This class holds all attributes who can affect by GameplayAbilitySystem 
 */
UCLASS()
class NETWORKSHOTER_API UNetShooterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UNetShooterAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
	/** Health */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MinHealth;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MinHealth)
	
	/** Armor */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MaxArmor)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MinArmor;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MinArmor)

	/** Speed */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WalkSpeed)
	FGameplayAttributeData WalkSpeed;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, WalkSpeed)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxWalkSpeed;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MaxWalkSpeed)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MinWalkSpeed;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MinWalkSpeed)

	/** Stamina */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MinStamina;
	ATTRIBUTE_ACCESSORS(UNetShooterAttributeSet, MinStamina)
	
protected:

///OnRep functions
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_WalkSpeed(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue);

	bool bOutOfHealth = false;
};
