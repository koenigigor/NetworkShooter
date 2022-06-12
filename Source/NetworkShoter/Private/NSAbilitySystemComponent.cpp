// Fill out your copyright notice in the Description page of Project Settings.


#include "NSAbilitySystemComponent.h"

#include "GAS/Ability/NSGameplayAbility.h"

namespace
{
	FGameplayTag GetRootTag(const FGameplayTag& Tag)
	{
		FGameplayTag RootTag = Tag;
		
		FGameplayTag NewRoot = RootTag.RequestDirectParent();
		while(NewRoot.IsValid())
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
		//UE_LOG(LogTemp, Display, TEXT("Input signal detect %s"), *EventTag.ToString())
		
		//cut Input/Release endings	//todo if setup advanced input delete endings cut
		if (EventTag.ToString().Contains("Press"))
		{
			InputTagPressed(EventTag); // cut inside after get handle
		}
		else if (EventTag.ToString().Contains("Release"))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Release tag detected %s"), *EventTag.ToString())
			InputTagReleased(EventTag.RequestDirectParent());
		}
		else
		{
			//input tag without Press/Release tags, temporary cam delete after move to advanced input and call  InputTagPressed/Released direct from there
			if (GameplayEventTriggeredAbilities.Contains(EventTag))
			{
				for (const auto& AbilityHandle : GameplayEventTriggeredAbilities[EventTag])
				{
					TryActivateAbility(AbilityHandle);
				}
			}
		}

		if (const FGameplayEventMulticastDelegate* Delegate = GenericGameplayEventCallbacks.Find(EventTag))
		{
			Delegate->Broadcast(Payload);
		} //for AbilityTask_WaitGameplayEvent
		
		return 0; 
	}
	
	return Super::HandleGameplayEvent(EventTag, Payload);
}

void UNSAbilitySystemComponent::InputTagPressed(const FGameplayTag& Tag)
{
	//iterate abilities
	//if input hold ability add to try activate array
	//if input press ability activate immediately

	//UE_LOG(LogTemp, Display, TEXT("Tag pressed"))
	
	if (GameplayEventTriggeredAbilities.Contains(Tag))
	{
		TArray<FGameplayAbilitySpecHandle> TriggeredAbilityHandles = GameplayEventTriggeredAbilities[Tag];

		for (const FGameplayAbilitySpecHandle& AbilityHandle : TriggeredAbilityHandles)
		{
			//on server we cant ProcessInputHold so just activate once it
			if (GetNetMode() == NM_DedicatedServer)
			{
				//UE_LOG(LogTemp, Display, TEXT("ability %s called on server, no look hold buttom, just activate once"), *Tag.ToString())
				TryActivateAbility(AbilityHandle);
				continue;
			}
			
			if (IsInputHoldAbility(AbilityHandle))
			{
				//UE_LOG(LogTemp, Display, TEXT("add in AbilitiesWhoWaitInputRelease"))
				AbilitiesWhoWaitInputRelease.FindOrAdd(Tag.RequestDirectParent(), AbilityHandle);
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
	//UE_LOG(LogTemp, Display, TEXT("%s try remove tag from map"), *Tag.ToString())
	AbilitiesWhoWaitInputRelease.Remove(Tag);
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
		{	//find new ability with this activation tag, or invalidate it
			if (GameplayEventTriggeredAbilities.Contains(AbilityHandle.Key))
			{
				AbilityHandle.Value = GameplayEventTriggeredAbilities[AbilityHandle.Key][0];
			}
			else
			{
				InvalidAbilities.Add(AbilityHandle.Key);
			}
		}
	}//mb iterator instead foreach, and remove iterator, be faster?
	
	for (const auto& InvalidTag : InvalidAbilities)
	{
		AbilitiesWhoWaitInputRelease.Remove(InvalidTag);
	}

	//todo OnGiveAbility(), check if input for this ability already pressed
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
