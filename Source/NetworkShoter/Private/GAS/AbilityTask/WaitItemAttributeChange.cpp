// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/WaitItemAttributeChange.h"

#include "Inventory/NSItemInstance.h"


UWaitItemAttributeChange* UWaitItemAttributeChange::AbilityTask_WaitItemAttributeChange(UGameplayAbility* OwningAbility, UNSItemInstance* Item,
																			FGameplayTag Attribute, bool TriggerOnce)
{
	auto Task = NewAbilityTask<UWaitItemAttributeChange>(OwningAbility);
	Task->Item = Item;
	Task->AttributeTag = Attribute;
	Task->bTriggerOnce = TriggerOnce;
	return Task;
}


void UWaitItemAttributeChange::Activate()
{
	Super::Activate();

	if (!Item) { EndTask(); return; }

	AttributeChangeDelegateHandle = Item->GetItemAttributeValueChangeDelegate(AttributeTag).AddUObject(this, &UWaitItemAttributeChange::OnAttributeChange);
	//any notifiers if item destroyed?
}

void UWaitItemAttributeChange::OnAttributeChange(const FGameplayTagAttribute& Attribute)
{
	OnChange.Broadcast(Attribute.GetTag(), Attribute.GetValue(), Attribute.GetPreviousValue());

	if (bTriggerOnce)
	{
		EndTask();
	}
}

void UWaitItemAttributeChange::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	if (!Item) { return; }
	Item->GetItemAttributeValueChangeDelegate(AttributeTag).Remove(AttributeChangeDelegateHandle);
}
