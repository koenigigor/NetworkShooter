// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask/AbilityTask_WaitGrenadeExplode.h"

#include "NetworkShoter/Weapon.h"

UAbilityTask_WaitGrenadeExplode::UAbilityTask_WaitGrenadeExplode()
{
	bTickingTask = true;
}

void UAbilityTask_WaitGrenadeExplode::Activate()
{
	if (Grenade)
	{
		LocationOnPreviousFrame = Grenade->GetActorLocation();
		Grenade->OnActorHit.AddDynamic(this, &UAbilityTask_WaitGrenadeExplode::OnHit);
	}
}

void UAbilityTask_WaitGrenadeExplode::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHit receive"))
	OnExplodeCallback(Hit.Location);
}

void UAbilityTask_WaitGrenadeExplode::TickTask(float DeltaTime)
{
	if(Grenade)
	{		
		//trace path betwen frames
		FHitResult OutHit;
		FCollisionQueryParams Query;
		Query.AddIgnoredActor(Grenade->GetInstigator());
		if (GetWorld()->LineTraceSingleByChannel(OutHit, LocationOnPreviousFrame, Grenade->GetActorLocation(), ECollisionChannel::ECC_Visibility, Query))
		{
			UE_LOG(LogTemp, Warning, TEXT("Tick Hit receive"))
			OnExplodeCallback(OutHit.Location);
		}

		LocationOnPreviousFrame = Grenade->GetActorLocation();
	}
}

UAbilityTask_WaitGrenadeExplode* UAbilityTask_WaitGrenadeExplode::WaitForExplode(UGameplayAbility* OwningAbility,
	AWeapon* GrenadeActor)
{
	auto abilityTask = NewAbilityTask<UAbilityTask_WaitGrenadeExplode>(OwningAbility);
	abilityTask->Grenade = GrenadeActor;
	return abilityTask;
}

void UAbilityTask_WaitGrenadeExplode::OnDestroy(bool bInOwnerFinished)
{
	if (Grenade)
		Grenade->OnActorHit.RemoveAll(this);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitGrenadeExplode::OnExplodeCallback(FVector Location)
{
	//auto TargetData = new FGameplayAbilityTargetData_LocationInfo();
	//auto Loc =  FGameplayAbilityTargetingLocationInfo();
	//Loc.LiteralTransform
	//TargetData->SourceLocation = Loc;

	//auto TargetdataHandle = FGameplayAbilityTargetDataHandle(TargetData);
	//OnExplode.Broadcast(TargetdataHandle);
	OnExplode.Broadcast(Location);
}
