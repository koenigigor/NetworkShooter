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

void UNSAbilitySystemComponent::SendInputReleaseToAbility(const FGameplayTagContainer& GameplayTagContainer, bool bReplicateOnServer)
{
	InputRelease.Broadcast(GameplayTagContainer);

	if (bReplicateOnServer)
		Server_SendInputReleaseToAbility(GameplayTagContainer);
}

void UNSAbilitySystemComponent::SendInputPressToAbility(const FGameplayTagContainer& GameplayTagContainer, bool bReplicateOnServer)
{
	InputPress.Broadcast(GameplayTagContainer);
	
	TryActivateAbilitiesByTag(GameplayTagContainer);

	if (bReplicateOnServer)
		Server_SendInputPressToAbility(GameplayTagContainer);
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

		//return 0; //not able catch tag by AbilityTask_WaitGameplayEvent
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

void UNSAbilitySystemComponent::ProcessInputHoldAbilities()
{
	for (const auto& AbilityHandle : AbilitiesWhoWaitInputRelease)
	{
		TryActivateAbility(AbilityHandle.Value);
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

void UNSAbilitySystemComponent::Server_SendInputReleaseToAbility_Implementation(
	const FGameplayTagContainer& GameplayTagContainer)
{
	SendInputReleaseToAbility(GameplayTagContainer);
}

void UNSAbilitySystemComponent::Server_SendInputPressToAbility_Implementation(
	const FGameplayTagContainer& GameplayTagContainer)
{
	SendInputPressToAbility(GameplayTagContainer);
}

/* or use todo ability inputs, and use replicate input directly */