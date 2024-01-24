// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GameplayTagAttributeContainer.h"

void FGameplayTagAttributeContainer::AddAttribute(FGameplayTag Tag, float Value)
{
	if (!Tag.IsValid() || Value <= 0.f) return;
		
	for (auto& Attribute : Attributes)
	{
		if (Attribute.GetTag() == Tag)
		{
			const float NewValue = Attribute.GetValue() + Value;
			Attribute.SetValue(NewValue);
			TagToCountMap[Tag] = NewValue;
			BroadcastAttributeChange(Attribute);
			
			MarkItemDirty(Attribute);
			return;
		}
	}
	
	//if not found add new item
	auto& NewItem = Attributes.Emplace_GetRef(Tag, Value);
	MarkItemDirty(NewItem);
	TagToCountMap.Add(Tag, Value);
	BroadcastAttributeChange(NewItem);
}

void FGameplayTagAttributeContainer::RemoveAttribute(FGameplayTag Tag, float Value)
{
	if (!Tag.IsValid() || Value <= 0.f) return;

	for (auto It = Attributes.CreateIterator(); It; ++It)
	{
		FGameplayTagAttribute& Attribute = *It;
		if (Attribute.GetTag() != Tag) continue; //next iteration
		
		if (Attribute.GetValue() <= Value)
		{
			Attribute.SetValue(0);	//set 0 for can broadcast value
			TagToCountMap.Remove(Tag);
			BroadcastAttributeChange(Attribute);
			It.RemoveCurrent();
			MarkArrayDirty();
		}
		else
		{
			const float NewValue = Attribute.GetValue() - Value;
			Attribute.SetValue(NewValue);
			TagToCountMap[Tag] = NewValue;
			MarkItemDirty(Attribute);
		}
		
		BroadcastAttributeChange(Attribute);
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

FItemAttributeChange& FGameplayTagAttributeContainer::GetItemAttributeValueChangeDelegate(FGameplayTag Attribute)
{
	return AttributeValueChangeDelegates.FindOrAdd(Attribute);
}

void FGameplayTagAttributeContainer::BroadcastAttributeChange(FGameplayTagAttribute Attribute)
{
	//if not contains then nobody not listen changes
	if (AttributeValueChangeDelegates.Contains(Attribute.GetTag()))
		AttributeValueChangeDelegates[Attribute.GetTag()].Broadcast(Attribute);
}

void FGameplayTagAttributeContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const auto& Index : RemovedIndices)
	{
		const FGameplayTag Tag = Attributes[Index].GetTag();
		TagToCountMap.Remove(Tag);
		BroadcastAttributeChange(Attributes[Index]);
	}
}

void FGameplayTagAttributeContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		const auto& Item = Attributes[Index];
		TagToCountMap.Add(Item.GetTag(), Item.GetValue());
		BroadcastAttributeChange(Item);
	}
}

void FGameplayTagAttributeContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		auto& Item = Attributes[Index];
		TagToCountMap[Item.GetTag()] = Item.GetValue();
		BroadcastAttributeChange(Item);
		
		Item.SetValue(Item.GetValue());	//update previous value
	}
}

