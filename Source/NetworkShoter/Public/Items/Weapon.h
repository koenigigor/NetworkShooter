// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UProjectileMovementComponent;

UENUM()
enum class EWeaponStatus : uint8
{
	InWorld,
	InStorage,
	Equipped
};

UCLASS()
class NETWORKSHOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void PostInitProperties() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Setup weapon data from datatable,
	 *  Must be called after construction */
	UFUNCTION(BlueprintCallable)
	virtual void SetupData(UWeaponData* WeaponData);

	/** set velocity and activate projectile movement */
	UFUNCTION(BlueprintCallable)
	void LaunchAsProjectile(FVector Velocity);

	/** [Server] change weapon visibility parameters, on change condition (equip, stored, etc)*/
	void SetStatus(EWeaponStatus NewStatus);

protected:
	UFUNCTION()
	void OnRep_Status();
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
    UWeaponData* WeaponData;
    
	UPROPERTY()
	UWeaponAttributeSet* WeaponAttributeSet = nullptr;	
	
protected:
	UPROPERTY() //Replicated)
	UStaticMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_Status)
	EWeaponStatus Status;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)  //todo need blueprint and visible?
	UProjectileMovementComponent* ProjectileMovement;	
};
