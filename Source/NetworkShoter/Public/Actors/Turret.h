// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAttitude.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

UCLASS()
class NETWORKSHOTER_API ATurret : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:	
	ATurret();

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintPure)
	EGameTeam GetTeamID_Verbose()
	{
		auto L_TeamID = GetGenericTeamId().GetId();
		return EGameTeam(L_TeamID);
	}
};
