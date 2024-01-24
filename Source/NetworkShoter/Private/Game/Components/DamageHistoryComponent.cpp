// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/DamageHistoryComponent.h"

#include "GenericTeamAgentInterface.h"
#include "NSStructures.h"
#include "Game/NSPlayerState.h"



UDamageHistoryComponent::UDamageHistoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<APlayerState*> UDamageHistoryComponent::GetAssist(const AActor* Target)
{
	TArray<APlayerState*> ResultArray;
	if (!Target && !DamageHistory.Contains(Target)) return ResultArray;

	for (auto DamageInfo : DamageHistory[Target])
	{
		ResultArray.AddUnique(DamageInfo.InstigatorState);
	}

	return ResultArray;
}


void UDamageHistoryComponent::BeginPlay()
{
	Super::BeginPlay();

	auto& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	DamageListener = MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DAMAGE, this, &ThisClass::OnDamage);
	DeathListener = MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DEATH, this, &ThisClass::OnDeath);
}

void UDamageHistoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	auto& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(DamageListener);
	MessageSubsystem.UnregisterListener(DeathListener);
}

void UDamageHistoryComponent::OnDamage(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{
	auto& DamageMap = DamageHistory.FindOrAdd(DamageInfo.Target);
	DamageMap.Add(DamageInfo);
}

void UDamageHistoryComponent::OnDeath(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{
	const auto TargetTeam = FGenericTeamId::GetTeamIdentifier(DamageInfo.Target);
	const auto InstigatorTeam = FGenericTeamId::GetTeamIdentifier(DamageInfo.InstigatorState);
	const auto Attitude = FGenericTeamId::GetAttitude(TargetTeam, InstigatorTeam);
	
	//add kill count
	if (DamageInfo.InstigatorState)
	{
		const auto NSPlayerState = Cast<ANSPlayerState>(DamageInfo.InstigatorState);
		ensure(NSPlayerState);

		if (Attitude != ETeamAttitude::Friendly)
		{
			NSPlayerState->AddKill();
		}
		else
		{
			NSPlayerState->AddKill(-1); 
		}
	}
    
	//add death count
	if (const auto NSPlayerState = DamageInfo.Target -> GetInstigator() -> GetPlayerState<ANSPlayerState>())
	{
		NSPlayerState -> AddDeath();
	}
        		
	//add assist count
	const auto Assistants = GetAssist(DamageInfo.Target);
	for (const auto& Assistant : Assistants)
	{
		//if !Friendly add assistant
		if (Assistant != DamageInfo.InstigatorState && FGenericTeamId::GetAttitude(DamageInfo.Target, Assistant) != ETeamAttitude::Friendly)
		{
			if (const auto NSPlayerState = Cast<ANSPlayerState>(Assistant))  //todo change DamageInfo PlayerState on NSPlayerState
				{
				NSPlayerState->AddAssist();
				}
		}
	}		
}

