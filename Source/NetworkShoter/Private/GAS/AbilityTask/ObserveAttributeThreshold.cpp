// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/ObserveAttributeThreshold.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


void UObserveAttributeThreshold::Activate()
{
	Super::Activate();

	UAbilitySystemComponent* ASC = GetFocusedASC();
	
	const float CurrentValue = ASC->GetNumericAttribute(Attribute);

	// Broadcast OnChange immediately with current value
	const float MaxValue = UpThreshold; //or pass max attribute?
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (CurrentValue < UpThreshold && CurrentValue > DownThreshold)
		{
			AttributeChangeInRange.Broadcast(MaxValue, CurrentValue);
		}
		if (CurrentValue >= UpThreshold)
		{
			UpThresholdReach.Broadcast(MaxValue, CurrentValue);
		}
		else if (CurrentValue <= DownThreshold)
		{
			DownThresholdReach.Broadcast(MaxValue, CurrentValue);
		}
	}

	OnAttributeChangeDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UObserveAttributeThreshold::OnAttributeChange);
}

void UObserveAttributeThreshold::OnDestroy(bool bInOwnerFinished)
{
	//remove delegate
	if (UAbilitySystemComponent* ASC = GetFocusedASC())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(OnAttributeChangeDelegateHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UObserveAttributeThreshold::OnAttributeChange(const FOnAttributeChangeData& CallbackData)
{
	const float OldValue = CallbackData.OldValue;
	const float CurrentValue = CallbackData.NewValue;

	bool bThresholdReached = false;
	
	if (CurrentValue < UpThreshold && CurrentValue > DownThreshold)
	{
		AttributeChangeInRange.Broadcast(OldValue, CurrentValue);
	}
	
	if ((CurrentValue >= UpThreshold && UpThreshold > OldValue) || (CurrentValue <= UpThreshold && UpThreshold < OldValue))
	{
		UpThresholdReach.Broadcast(OldValue, CurrentValue);
		bThresholdReached = true;
	}
	else if ((OldValue > DownThreshold &&  DownThreshold >= CurrentValue) || (OldValue < DownThreshold &&  DownThreshold <= CurrentValue))
	{
		DownThresholdReach.Broadcast(OldValue, CurrentValue);
		bThresholdReached = true;
	}
	
	if (bTriggerOnce && bThresholdReached)
	{
		EndTask();
	}
}


UObserveAttributeThreshold* UObserveAttributeThreshold::ObserveAttributeThreshold(UGameplayAbility* OwningAbility,
                                                                                  FGameplayAttribute ObservedAttribute, float UpThresholdValue, float DownThresholdValue, bool TriggerOnce,
                                                                                  AActor* OptionalExternalOwner)
{
	UObserveAttributeThreshold* Task = NewAbilityTask<UObserveAttributeThreshold>(OwningAbility);

	Task->Attribute = ObservedAttribute;
	Task->UpThreshold = UpThresholdValue;
	Task->DownThreshold = DownThresholdValue;
	Task->bTriggerOnce = TriggerOnce;
	Task->ExternalOwner = OptionalExternalOwner;

	return Task;
}


UAbilitySystemComponent* UObserveAttributeThreshold::GetFocusedASC()
{
	if (!ExternalOwner)
	{
		return AbilitySystemComponent.Get();
	}

	auto AbilityInterface = Cast<IAbilitySystemInterface>(ExternalOwner);
	check(AbilityInterface)
	return AbilityInterface->GetAbilitySystemComponent();
}
