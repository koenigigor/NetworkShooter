// Fill out your copyright notice in the Description page of Project Settings.


#include "GCImpact.h"


#include "NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

bool UGCImpact::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const auto Location = Parameters.Location;
	const auto Rotation = Parameters.Normal.Rotation();	
	const auto ImpactFX = GetImpactFX(Parameters.PhysicalMaterial.Get());

	//draw beam if is trace hit
	if (Parameters.TargetAttachComponent.IsValid())
	{
		const auto Muzzle = Parameters.TargetAttachComponent.Get()->GetSocketLocation("Muzzle");
		if (const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceBeamFX, Muzzle))
		{
			TraceFX->SetVectorParameter("EndPoint", Location);
		}
	}

	//Draw decal if is blocking hit	
	if (Parameters.PhysicalMaterial.IsValid())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, Location, Rotation);
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, DecalSize, Location, Rotation, DecalLifeSpan);
	}
	
	return Super::OnExecute_Implementation(MyTarget, Parameters);
}

UNiagaraSystem* UGCImpact::GetImpactFX(const UPhysicalMaterial* Material) const
{
	auto ImpactFX = DefaultImpact;
	if (ImpactMap.Contains(Material))
	{
		ImpactFX = ImpactMap[Material];
	}

	return ImpactFX;
}
