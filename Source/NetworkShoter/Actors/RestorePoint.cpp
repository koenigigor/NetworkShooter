// Fill out your copyright notice in the Description page of Project Settings.


#include "RestorePoint.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


ARestorePoint::ARestorePoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ARestorePoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bCollected);
}

FString ARestorePoint::GetDamageDescription_Implementation(const FDamageInfo& DamageInfo) const
{
	return FString::Printf(TEXT("%s enter and restoring point %s"),
		*DamageInfo.Target->GetInstigator()->GetPlayerState()->GetPlayerName(),
		*GetName());
}

void ARestorePoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!EffectClass) return;

	if (HasAuthority() && !bCollected)
	{
		if (const auto AbilityInterface = Cast<IAbilitySystemInterface>(OtherActor))
		{
			//apply effect
			const auto ASC = AbilityInterface->GetAbilitySystemComponent();

			auto EffectContext = ASC->MakeEffectContext();
			EffectContext.AddInstigator(EffectContext.GetInstigator(), this);
			const auto EffectSpec = ASC->MakeOutgoingSpec(EffectClass, 1.f, EffectContext);
			ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

			bCollected = true;
			OnCollect();
		}
	}
}

bool ARestorePoint::IsOnRestore() const
{
	return GetWorldTimerManager().TimerExists(RestoreTimer);
}

void ARestorePoint::OnCollect_Implementation()
{
	if (IsOnRestore()) return;

	GetWorldTimerManager().SetTimer(RestoreTimer, this, &ThisClass::OnRestore, RestoreTime);

	BP_OnCollect();
}

void ARestorePoint::OnRestore_Implementation()
{
	bCollected = false;
	BP_OnRestore();
}

void ARestorePoint::OnRep_bCollected()
{
	if (bCollected)
		OnCollect();
}
