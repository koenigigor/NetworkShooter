// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask/ThrowWeaponAsProjectile.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkShoter/Weapon.h"

UThrowWeaponAsProjectile::UThrowWeaponAsProjectile()
{
	bTickingTask=true;
}

void UThrowWeaponAsProjectile::Activate()
{
	Super::Activate();

	if (!Weapon){ return; };

	//TODO GameplayAbilityTypes FGameplayAbilityActorInfo::InitFromActor() for get skeletal mesh component (character interface)
	
	//set location //todo
	//(GetOwnerActor()->FindComponentByClass<USkeletalMeshComponent>())->GetSocketLocation("Gun");
	
	
	//Get owner view point
	FVector ViewStart;
	FRotator ViewRotation;
	FHitResult ViewHit;
	GetOwnerActor()->GetInstigator()->GetController()->GetActorEyesViewPoint(ViewStart, ViewRotation);
	FVector ViewEnd = ViewRotation.Vector() * 99999.f;
	GetWorld()->LineTraceSingleByChannel(ViewHit, ViewStart, ViewEnd, ECollisionChannel::ECC_Visibility);

	FVector ProjectileEndLocation = ViewHit.bBlockingHit ? ViewHit.Location : ViewHit.TraceEnd;
	FVector ProjectileStartLocation = Weapon->GetActorLocation();

	if (bSetRotationAsOwnerView)
	{
		Weapon->SetActorRotation(ViewRotation);
	}
	
	//suggest projectile velocity
	float ProjectileSpeed = Weapon->WeaponData->ThrowSpeed;
	float ProjectileAcceptRadius = Weapon->WeaponData->ExplodeRadius;
	FVector ProjectileVelocity;
	auto bVelocityFound = UGameplayStatics::SuggestProjectileVelocity(GetWorld(),
		ProjectileVelocity,
		ProjectileStartLocation, 
		ProjectileEndLocation,
		ProjectileSpeed,
		false,
		ProjectileAcceptRadius);
	
	//Launch to owner view point
	if (bVelocityFound)
	{
		//launch suggest velocity
		Weapon->LaunchAsProjectile(ProjectileVelocity);
	}
	else
	{
		//launch player view
		ProjectileVelocity = ViewRotation.Vector().GetSafeNormal() * ProjectileSpeed;
		Weapon->LaunchAsProjectile(ProjectileVelocity);
	};

	LocationOnPreviousFrame = Weapon->GetActorLocation();
	Weapon->OnActorHit.AddDynamic(this, &UThrowWeaponAsProjectile::OnHit);
}

UThrowWeaponAsProjectile* UThrowWeaponAsProjectile::AbilityTask_ThrowWeaponAsProjectile(UGameplayAbility* OwningAbility,
                                                                                        AWeapon* ThrowingWeapon, bool SetRotationAsOwnerView)
{
	auto Task = NewAbilityTask<UThrowWeaponAsProjectile>(OwningAbility);
	Task->Weapon = ThrowingWeapon;
	Task->bSetRotationAsOwnerView = SetRotationAsOwnerView;
	return Task;
}

void UThrowWeaponAsProjectile::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(Weapon)
	{		
		//trace path between frames
		FHitResult OutHit;
		FCollisionQueryParams Query;
		Query.AddIgnoredActor(Weapon->GetInstigator());
		Query.AddIgnoredActor(Weapon);
		
		DrawDebugLine(GetWorld(), LocationOnPreviousFrame, Weapon->GetActorLocation(), FColor::Green, false, 10.f, 2, 5);
		
		if (GetWorld()->LineTraceSingleByChannel(OutHit, LocationOnPreviousFrame, Weapon->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel2, Query))
		{
			OnHit(nullptr, nullptr, FVector::UpVector, OutHit);
		}
		
		LocationOnPreviousFrame = Weapon->GetActorLocation();
	}
}

void UThrowWeaponAsProjectile::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	// Construct TargetData
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);

	// Give it a handle and return
	FGameplayAbilityTargetDataHandle Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));
	
	WeaponHit.Broadcast(Handle);
}
