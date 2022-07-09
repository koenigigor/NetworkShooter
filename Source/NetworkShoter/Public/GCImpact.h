// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCImpact.generated.h"

class UNiagaraSystem;


/** GameplayCue for shooting, spawn niagara system and decal on hit location */
UCLASS()
class NETWORKSHOTER_API UGCImpact : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

	
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
	
protected:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* DefaultImpact = nullptr;

	//for weapons shooting by trace
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TraceBeamFX = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<UPhysicalMaterial*, UNiagaraSystem*> ImpactMap;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Decal = nullptr;

	UPROPERTY(EditAnywhere)
	FVector DecalSize = FVector(1.f);

	UPROPERTY(EditAnywhere)
	float DecalLifeSpan = 5.f;

private:
	UNiagaraSystem* GetImpactFX(const UPhysicalMaterial* Material) const;
};
