// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AsyncTaskCooldownChanged.h"

UAsyncTaskCooldownChanged* UAsyncTaskCooldownChanged::ListenForCooldownChange(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer CooldownTag, const bool UseServerCooldown)
{
	//protect invalid data
	if (!AbilitySystemComponent || CooldownTag.Num() == 0)
	{
		return nullptr;
	}

	//construct task
	UAsyncTaskCooldownChanged* ListenTask = NewObject<UAsyncTaskCooldownChanged>();
	ListenTask->AbilitySystem = AbilitySystemComponent;
	ListenTask->CooldownTags = CooldownTag;
	ListenTask->bUseServerCooldown = UseServerCooldown;

	//bind on add new gameplay effect
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenTask, &UAsyncTaskCooldownChanged::OnActiveGameplayEffectAddedCallback);

	//bind to tag removed (cooldown ends)		//array if we listen different cooldowns
	TArray<FGameplayTag> CooldownTagArray;
	CooldownTag.GetGameplayTagArray(CooldownTagArray);
	for (auto& Tag : CooldownTagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenTask, &UAsyncTaskCooldownChanged::CooldownTagChanged);
	}

	return ListenTask;
}

void UAsyncTaskCooldownChanged::EndTask()
{
	//unbind delegates
	if (AbilitySystem)
	{
		AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		
		TArray<FGameplayTag> CooldownTagArray;
		CooldownTags.GetGameplayTagArray(CooldownTagArray);
		for (auto& Tag : CooldownTagArray)
		{
			AbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		} 
	}

	//destruct object
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsyncTaskCooldownChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	TArray<FGameplayTag> CooldownTagArray;
	CooldownTags.GetGameplayTagArray(CooldownTagArray);	

	//foreach listening cooldowns
	for (auto& CooldownTag : CooldownTagArray)
	{
		//we observe this tag?
		if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
		{
			float TimeRemaining = 0.0f;
			float Duration = 0.0f;

			//expect cooldown tag be first tag			//?????
			FGameplayTagContainer CooldownTagContainer(GrantedTags.GetByIndex(0));
			
			GetCooldownRemainingForTag(CooldownTagContainer, TimeRemaining, Duration);
			
			if (AbilitySystem->GetOwnerRole() == ROLE_Authority)
			{
				// Player is Server
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (!bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			{
				// Client using predicted cooldown
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr)
			{
				// Client using Server's cooldown. This is Server's corrective cooldown GE.
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			{
				// Client using Server's cooldown but this is predicted cooldown GE.
				// This can be useful to gray out abilities until Server's cooldown comes in.
				OnCooldownBegin.Broadcast(CooldownTag, -1.0f, -1.0f);
			}
		}
	}
}

void UAsyncTaskCooldownChanged::CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnd.Broadcast(CooldownTag, -1.0f, -1.0f);
	}
}

bool UAsyncTaskCooldownChanged::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTag, float& TimeRemaining,
	float& CooldownDuration)
{
	if (!IsValid(AbilitySystem) || CooldownTag.Num() == 0){	return false; }
	
	TimeRemaining = 0.f;
	CooldownDuration = 0.f;
	
	//get all effects with duration, and our tag
	FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag);
	auto DurationAndTimeRemaining = AbilitySystem->GetActiveEffectsTimeRemainingAndDuration(Query);

	if (DurationAndTimeRemaining.Num() == 0) {return false; }

	//if we has multiple cooldowns with same tag, get longest
	int32 LongestTimeIndex = 0;
	float LongestTime = DurationAndTimeRemaining[0].Key;
	for (int32 i = 1; i < DurationAndTimeRemaining.Num(); i++)
	{
		if (DurationAndTimeRemaining[i].Key > LongestTime)
		{
			LongestTime = DurationAndTimeRemaining[i].Key;
			LongestTimeIndex = i;
		}
	}

	TimeRemaining = DurationAndTimeRemaining[LongestTimeIndex].Key;
	CooldownDuration = DurationAndTimeRemaining[LongestTimeIndex].Value;

	return true;
}




