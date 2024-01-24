// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCHitReaction.generated.h"

/**
 * gameplay cie for character reaction on hit,
 * parameters contains
 * incoming damage (Parameters.RawMagnitude)
 * direction to enemy (Parameters.Normal)
 * phis material, and effect context
 */
UCLASS()
class NETWORKSHOTER_API UGCHitReaction : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly, Meta = (MustImplement = "/Script/Engine.CameraLensEffectInterface"))
	TSubclassOf<AActor> LensEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> UIEffect;
};
