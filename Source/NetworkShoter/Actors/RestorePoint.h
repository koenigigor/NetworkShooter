// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DamageDescriptionInterface.h"
#include "RestorePoint.generated.h"


/** Actor response for restoring point, (HP, Armor), on overlap */
UCLASS()
class NETWORKSHOTER_API ARestorePoint : public AActor, public IDamageDescriptionInterface
{
	GENERATED_BODY()
public:
	ARestorePoint();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	float RestoreTime = 1.f;

	/** Effect apply on interacted actor */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UGameplayEffect> EffectClass = nullptr;


public:
	virtual FString GetDamageDescription_Implementation(const FDamageInfo& DamageInfo) const override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** Point on restore CD or not */
	bool IsOnRestore() const;

protected:
	UFUNCTION(NetMulticast, Reliable)
	void OnRestore();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnRestore();

	UFUNCTION(NetMulticast, Reliable)
	void OnCollect();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCollect();

	//for players connected in match 
	UPROPERTY(ReplicatedUsing="OnRep_bCollected")
	bool bCollected = false;
	UFUNCTION()
	void OnRep_bCollected();

	FTimerHandle RestoreTimer;
};
