// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GameplayTagAttributeContainer.h"

void FGameplayTagAttributeContainer::AddAttribute(FGameplayTag Tag, float Value)
{
	if (!Tag.IsValid() || Value <= 0.f) return;
		
	for (auto& Attribute : Attributes)
	{
		if (Attribute.Tag == Tag)
		{
			const float NewValue = Attribute.Value + Value;
			Attribute.Value = NewValue;
			TagToCountMap[Tag] = NewValue;
			MarkItemDirty(Attribute);
			return;
		}
	}
	
	//if not found add new item
	auto& NewItem = Attributes.Emplace_GetRef(Tag, Value);
	MarkItemDirty(NewItem);
	TagToCountMap.Add(Tag, Value);
}

void FGameplayTagAttributeContainer::RemoveAttribute(FGameplayTag Tag, float Value)
{
	if (!Tag.IsValid() || Value <= 0.f) return;

	for (auto It = Attributes.CreateIterator(); It; ++It)
	{
		FGameplayTagAttribute& Attribute = *It;
		if (Attribute.Tag != Tag) continue; //next iteration
		
		if (Attribute.Value <= Value)
		{
			It.RemoveCurrent();
			TagToCountMap.Remove(Tag);
			MarkArrayDirty();
		}
		else
		{
			const float NewValue = Attribute.Value - Value;
			Attribute.Value = NewValue;
			TagToCountMap[Tag] = NewValue;
			MarkItemDirty(Attribute);
		}
		return;
	}
}

float FGameplayTagAttributeContainer::GetAttribute(FGameplayTag Tag) const
{
	return TagToCountMap.FindRef(Tag);
}

bool FGameplayTagAttributeContainer::ContainAttribute(FGameplayTag Tag) const
{
	return TagToCountMap.Contains(Tag);
}

void FGameplayTagAttributeContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const auto& Index : RemovedIndices)
	{
		const FGameplayTag Tag = Attributes[Index].Tag;
		TagToCountMap.Remove(Tag);
	}
}

void FGameplayTagAttributeContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		const auto& Item = Attributes[Index];
		TagToCountMap.Add(Item.Tag, Item.Value);
	}
}

void FGameplayTagAttributeContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		const auto& Item = Attributes[Index];
		TagToCountMap[Item.Tag] = Item.Value;
	}
}

