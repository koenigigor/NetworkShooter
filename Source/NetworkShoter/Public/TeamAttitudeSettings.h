// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamAttitude.h"
#include "Engine/DeveloperSettings.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAttitudeSettings.generated.h"


/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class NETWORKSHOTER_API UTeamAttitudeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Artificial Intelligence", EditAnywhere, Config, meta=(ArraySizeEnum="EGameTeam"))
	FTeamAttitude TeamAttitudes[EGameTeam::Num];
	

	UTeamAttitudeSettings(const FObjectInitializer& ObjectInitializer);

	static const UTeamAttitudeSettings* Get();

	UFUNCTION(Category = "Artificial Intelligence", BlueprintPure)
	static ETeamAttitude::Type GetAttitude(FGenericTeamId Of, FGenericTeamId Towards);
};
