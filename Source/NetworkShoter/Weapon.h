// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapon.generated.h"



UCLASS()
class NETWORKSHOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	/** Setup weapon data from datatable,
	 *  Must be called after construction */
	UFUNCTION(BlueprintCallable)
	virtual void SetupData(UWeaponData* WeaponData);

	UPROPERTY(BlueprintReadOnly)
	UWeaponData* WeaponData;

	/** set velocity and activate projectile movement */
	UFUNCTION(BlueprintCallable)
	void LaunchAsProjectile(FVector Velocity);
	
	/** Original weapon skill
	 *	slice for sword,
	 *	shoot for rifle, etc. */
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf<UGameplayAbility> PrimaryAbility;

	/** Second Weapon skill
	 * Block or punch for sword
	 * punch for rifle, etc */
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf<UGameplayAbility> SecondaryAbility;

	/** Throw weapon as projectile
	 *  Last word in hopeless situation =) */
//	UPROPERTY(EditDefaultsOnly)
//	TSubclassOf<UGameplayAbility> Throw;


protected:
	//UPROPERTY(Replicated)
	UStaticMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

/*	UPROPERTY(BlueprintReadWrite)
	bool bAttackingMelee = false;
	bool bPunchOnDelay = false;*/
/*
public:
	UPROPERTY(BlueprintReadWrite, Category="Default", meta = (ExposeOnSpawn=true))
	UWeaponData* WeaponDataVar = nullptr;*/
};
