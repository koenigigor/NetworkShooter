// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurretRotation.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UTurretRotation : public UActorComponent
{
	GENERATED_BODY()
public:
	UTurretRotation();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Init(USceneComponent* Turret, USceneComponent* Barrel);
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget);

protected:
	void UpdateResultRotation();
	void RotateTurret(float DeltaTime);
	void RotateBarrel(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	bool bDrawDebug = true;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float BarrelMinAngle = -15.f;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float BarrelMaxAngle = 70.f;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float BarrelRotationSpeed = 70.f;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float TurretMinAngle = -180.f;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float TurretMaxAngle = 180.f;
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float TurretRotationSpeed = 70.f;

	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle))
	bool bHasProjectile = false;
	UPROPERTY(EditDefaultsOnly, Category="Setup", meta=(EditCondition="bHasProjectile"))
	float ProjectileSpeed = 1000.f;

	UPROPERTY()
	USceneComponent* TurretRef = nullptr;
	UPROPERTY()
	USceneComponent* BarrelRef = nullptr;
	UPROPERTY()
	AActor* Target = nullptr;

	FRotator ResultRotation;
};
