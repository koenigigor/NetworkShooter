// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/NSAbilitySystemComponent.h"

#include "GAS/Ability/NSGameplayAbility.h"

namespace
{
FGameplayTag GetRootTag(const FGameplayTag& Tag)
{
	FGameplayTag RootTag = Tag;

	FGameplayTag NewRoot = RootTag.RequestDirectParent();
	while (NewRoot.IsValid())
	{
		RootTag = NewRoot;
		NewRoot = RootTag.RequestDirectParent();
	}
	return RootTag;
}
}


int32 UNSAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (GetRootTag(EventTag).ToString() == "Input")
	{
		//todo deprecated
		if (EventTag.ToString().Contains("Press"))
		{
			InputTagPressed(EventTag.RequestDirectParent());
		}
		else if (EventTag.ToString().Contains("Release"))
		{
			InputTagReleased(EventTag.RequestDirectParent());
		}
		else
		{
			InputTagPressed(EventTag);
		}

		if (const FGameplayEventMulticastDelegate* Delegate = GenericGameplayEventCallbacks.Find(EventTag))
		{
			Delegate->Broadcast(Payload);
		} //for AbilityTask_WaitGameplayEvent

		return 0;
	}

	return Super::HandleGameplayEvent(EventTag, Payload);
}

void UNSAbilitySystemComponent::InputTagPressed(const FGameplayTag& Tag, bool bActivateAbility)
{
	//iterate abilities
	//if input hold ability add in active abilities list
	//if input press ability activate immediately

	if (GameplayEventTriggeredAbilities.Contains(Tag))
	{
		TArray<FGameplayAbilitySpecHandle> TriggeredAbilityHandles = GameplayEventTriggeredAbilities[Tag];

		for (const FGameplayAbilitySpecHandle& AbilityHandle : TriggeredAbilityHandles)
		{
			//InputPressedEvent		based on UAbilitySystemComponent::AbilityLocalInputPressed
			if (const auto SpecPtr = FindAbilitySpecFromHandle(AbilityHandle))
			{
				auto& Spec = *SpecPtr;
				AbilitySpecInputPressed(Spec);

				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
			}

			//activate ability
			if (bActivateAbility)
				if (GetNetMode() != NM_DedicatedServer && IsInputHoldAbility(AbilityHandle))
				{
					AbilitiesWhoWaitInputRelease.FindOrAdd(Tag, AbilityHandle);
				}
				else
				{
					TryActivateAbility(AbilityHandle);
				}
		}
	}
}

void UNSAbilitySystemComponent::InputTagReleased(const FGameplayTag& Tag)
{
	AbilitiesWhoWaitInputRelease.Remove(Tag);

	//input release event, (copy from UAbilitySystemComponent::AbilityLocalInputReleased)
	if (GameplayEventTriggeredAbilities.Contains(Tag))
	{
		TArray<FGameplayAbilitySpecHandle> TriggeredAbilityHandles = GameplayEventTriggeredAbilities[Tag];

		for (const FGameplayAbilitySpecHandle& AbilityHandle : TriggeredAbilityHandles)
		{
			if (const auto SpecPtr = FindAbilitySpecFromHandle(AbilityHandle))
			{
				auto& Spec = *SpecPtr;

				Spec.InputPressed = false;
				if (Spec.Ability && Spec.IsActive())
				{
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputReleased(Spec.Handle);
					}

					AbilitySpecInputReleased(Spec);

					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
			}
		}
	}
}

bool UNSAbilitySystemComponent::AbilityIsActive(const FGameplayTagContainer& Tag)
{
	TArray<FGameplayAbilitySpec*> Abilities;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(Tag, Abilities);
	if (Abilities.IsValidIndex(0))
	{
		return Abilities[0]->IsActive();
	}
	
	return false;
}

void UNSAbilitySystemComponent::ServerReplicateGameplayEvent_Implementation(const FGameplayTag& Tag)
{
	const FGameplayEventData Payload;
	HandleGameplayEvent(Tag, &Payload);
}

void UNSAbilitySystemComponent::ServerReplicateSubclass_Implementation(FGameplayAbilitySpecHandle AbilityHandle,
	TSubclassOf<UObject> ObjectClass)
{
	OnClassReplicate.Broadcast(AbilityHandle, ObjectClass);
}

void UNSAbilitySystemComponent::ProcessInputHoldAbilities()
{
	TArray<FGameplayTag> InvalidAbilities;
	for (auto& AbilityHandle : AbilitiesWhoWaitInputRelease)
	{
		if (FindAbilitySpecFromHandle(AbilityHandle.Value))
		{
			TryActivateAbility(AbilityHandle.Value);
		}
		else
		{
			//find new ability with this activation tag, or invalidate it
			if (GameplayEventTriggeredAbilities.Contains(AbilityHandle.Key))
			{
				AbilityHandle.Value = GameplayEventTriggeredAbilities[AbilityHandle.Key][0];
			}
			else
			{
				InvalidAbilities.Add(AbilityHandle.Key);
			}
		}
	} //mb iterator instead foreach, and remove iterator, be faster?

	for (const auto& InvalidTag : InvalidAbilities)
	{
		AbilitiesWhoWaitInputRelease.Remove(InvalidTag);
	}
}

bool UNSAbilitySystemComponent::IsInputHoldAbility(const FGameplayAbilitySpecHandle& Handle)
{
	auto Spec = FindAbilitySpecFromHandle(Handle);
	if (!Spec || !Spec->Ability) return false;
	if (auto NSAbility = Cast<UNSGameplayAbility>(Spec->Ability))
	{
		return NSAbility->GetActivationPolicy() == EActivationPolicy::InputHold;
	}

	return false;
}
