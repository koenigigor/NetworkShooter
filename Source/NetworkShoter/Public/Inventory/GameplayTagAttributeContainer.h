// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAttributeContainer.generated.h"

//build.cs "NetCore"

struct FGameplayTagAttributeContainer;
struct FGameplayTagAttribute;

DECLARE_MULTICAST_DELEGATE_OneParam(FItemAttributeChange, const FGameplayTagAttribute& Attribute);

/**
 * One tag row, tag + float attribute
 */
USTRUCT()
struct FGameplayTagAttribute : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	FGameplayTagAttribute()
	{}

	FGameplayTagAttribute(FGameplayTag InTag, int32 InValue)
		: Tag(InTag)
		, Value(InValue)
	{
	}

	float GetValue() const { return Value; };
	float GetPreviousValue() const { return PreviousValue; };
	void SetValue(float NewValue) { PreviousValue = Value; Value = NewValue; };

	FGameplayTag GetTag() const { return Tag; };
	
private:
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	float Value = 0.f;
	
	UPROPERTY(NotReplicated)
	float PreviousValue = 0.f;
};


/**
 * container for tag and attribute
 */
USTRUCT()
struct FGameplayTagAttributeContainer : public FFastArraySerializer
{
	GENERATED_BODY()
	
	void AddAttribute(FGameplayTag Tag, float Value);

	void RemoveAttribute(FGameplayTag Tag, float Value);

	float GetAttribute(FGameplayTag Tag) const;

	bool ContainAttribute(FGameplayTag Tag) const;

	FItemAttributeChange& GetItemAttributeValueChangeDelegate(FGameplayTag Attribute);

private:
	void BroadcastAttributeChange(FGameplayTagAttribute Attribute);
	
public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FGameplayTagAttribute, FGameplayTagAttributeContainer>(Attributes, DeltaParams, *this);
	}


private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FGameplayTagAttribute> Attributes;

	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, float> TagToCountMap;

	TMap<FGameplayTag, FItemAttributeChange> AttributeValueChangeDelegates;
};

template<>
struct TStructOpsTypeTraits<FGameplayTagAttributeContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagAttributeContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
