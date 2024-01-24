// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/EquipmentAttributes.h"

#include "UObject/ObjectSaveContext.h"

UEquipmentAttributes::UEquipmentAttributes()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
}

void UEquipmentAttributes::OverrideModifiers()
{
	Modifiers.Empty();
	
	int32 ModifierIndex = 0;
 	Modifiers.SetNum(AttributesToApply.Num());
 
 	for (const auto& ToApply : AttributesToApply)
 	{
 		FGameplayModifierInfo& Modifier = Modifiers[ModifierIndex];
 		Modifier.ModifierMagnitude = FScalableFloat(ToApply.Value.Value);
 		Modifier.ModifierOp = ToApply.Value.ApplyMethod;
 		Modifier.Attribute = ToApply.Key;
  		
 		++ModifierIndex;
 	}
}

void UEquipmentAttributes::PreSave(FObjectPreSaveContext SaveContext)
{
	OverrideModifiers();
	
	Super::PreSave(SaveContext);
}