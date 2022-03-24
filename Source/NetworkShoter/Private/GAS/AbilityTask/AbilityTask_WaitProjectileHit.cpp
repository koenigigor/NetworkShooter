// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/AbilityTask_WaitProjectileHit.h"

#include "GameFramework/ProjectileMovementComponent.h"

UAbilityTask_WaitProjectileHit::UAbilityTask_WaitProjectileHit()
{
	bTickingTask = true;
}

void UAbilityTask_WaitProjectileHit::Activate()
{
	Super::Activate();

	if (!Projectile) return;
	LocationOnPreviousFrame = Projectile->GetActorLocation();
	Projectile->OnActorHit.AddDynamic(this, &UAbilityTask_WaitProjectileHit::ReceiveHit);
}

void UAbilityTask_WaitProjectileHit::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	if (!Projectile) return;
	Projectile->OnActorHit.RemoveAll(this);
}

UAbilityTask_WaitProjectileHit* UAbilityTask_WaitProjectileHit::AbilityTask_WaitProjectileHit(
	UGameplayAbility* OwningAbility, AActor* Actor)
{
	auto ProjectileMovement = Actor->FindComponentByClass<UProjectileMovementComponent>();
	if (!ensure(ProjectileMovement))
	{
		UE_LOG(LogTemp, Error, TEXT("Actor not contein Projectile movement component"))
		return nullptr;
	}
	if (ProjectileMovement->IsActive())
	{
		UE_LOG(LogTemp, Error, TEXT("Actor Projectile movement not active"))
		ProjectileMovement->Activate();
		//return nullptr;
	}
	
	auto abilityTask = NewAbilityTask<UAbilityTask_WaitProjectileHit>(OwningAbility);
	abilityTask->Projectile = Actor;
	return abilityTask;
}

void UAbilityTask_WaitProjectileHit::ReceiveHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile hit : %s"), *Hit.Actor->GetName())
	
	// Construct TargetData
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
	FGameplayAbilityTargetDataHandle Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
	OnHit.Broadcast(Handle);
	
	EndTask();
}

void UAbilityTask_WaitProjectileHit::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(!Projectile) {return;}
	
	//trace path betwen frames
	FHitResult OutHit;
	FCollisionQueryParams Query;
	Query.AddIgnoredActor(Projectile->GetInstigator());
	Query.AddIgnoredActor(Projectile);
	if (GetWorld()->LineTraceSingleByChannel(OutHit, LocationOnPreviousFrame, Projectile->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel2, Query))
	{
		ReceiveHit(Projectile, OutHit.GetActor(), OutHit.Normal, OutHit);
	}

	LocationOnPreviousFrame = Projectile->GetActorLocation();
}
