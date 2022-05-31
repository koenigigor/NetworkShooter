// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EFragment_Attributes.h"

#include "AbilitySystemComponent.h"
#include "Equipment/NSEquipmentInstance.h"
#include "GAS/EquipmentAttributes.h"

void UEFragment_Attributes::OnEquip(UNSEquipmentInstance* Instance)
{
	Super::OnEquip(Instance);
	if (!EquipmentAttributes) return;
	if (!Instance || !Instance->Instigator) return;

	if (!Instance->Instigator->HasAuthority()) return;
	
	const auto ASC = Instance->Instigator->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC) return;

	const auto SpecHandle = ASC->MakeOutgoingSpec(EquipmentAttributes, 1.f , ASC->MakeEffectContext());
	Instance->ApplyAttributesEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UEFragment_Attributes::OnUnequip(UNSEquipmentInstance* Instance)
{
	Super::OnUnequip(Instance);

	if (!Instance || !Instance->Instigator) return;

	const auto ASC = Instance->Instigator->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC) return;
	
	ASC->RemoveActiveGameplayEffect(Instance->ApplyAttributesEffectHandle);
}

/*
///only Instant effects can be create like that (set num modifiers in runtime),
///it work but log replication errors 
///LogAbilitySystem: Error: FActiveGameplayEffect::PostReplicatedAdd Received ReplicatedGameplayEffect with no UGameplayEffect def.
///LogAbilitySystem: Error: Received PreReplicatedRemove with no UGameplayEffect def.
///instead this return to idea make single effect to single weapon

USTRUCT()
struct FAttributeApply
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Value = 1;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EGameplayModOp::Type> ApplyMethod;
};

UPROPERTY(EditDefaultsOnly)
TMap<FGameplayAttribute, FAttributeApply> AttributesToApply;


// Create a dynamic Gameplay Effect to give the attributes //look in GasSample UGDAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data) 
UGameplayEffect* AttributesEffect = NewObject<UGameplayEffect>(ASC->GetOwner(), FName(TEXT("ApplyAttributesEffect")));
//AttributesEffect->DurationPolicy = EGameplayEffectDurationType::Infinite; 

int32 ModifierIndex = AttributesEffect->Modifiers.Num();
AttributesEffect->Modifiers.SetNum(ModifierIndex + AttributesToApply.Num());

for (const auto& ToApply : AttributesToApply)
{
	FGameplayModifierInfo& Modifier = AttributesEffect->Modifiers[ModifierIndex];
	Modifier.ModifierMagnitude = FScalableFloat(ToApply.Value.Value);
	Modifier.ModifierOp = ToApply.Value.ApplyMethod;
	Modifier.Attribute = ToApply.Key;

	++ModifierIndex;
}

Instance->ApplyAttributesEffectHandle = ASC->ApplyGameplayEffectToSelf(AttributesEffect, 1.f, ASC->MakeEffectContext());*/