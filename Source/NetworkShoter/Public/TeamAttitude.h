// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAttitude.generated.h"

/**
 * find there
 * https://forums.unrealengine.com/t/how-do-i-use-the-ai-perception-teams/120837/2
 * change for get verbose representation
 * meta=(ShowOnlyInnerProperties), not fount how add it
 */

UENUM(BlueprintType)
enum class EGameTeam : uint8
{
	Neutral UMETA(DisplayName = "Neutral"),
	Team1 UMETA(DisplayName = "Team1"),
	Team2 UMETA(DisplayName = "Team2"),
	Num UMETA(Hidden) //must be last item
};

USTRUCT(BlueprintType, meta=(ScriptName="FTeamAttitude"))
struct FTeamAttitude
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(ArraySizeEnum="EGameTeam"))
	TEnumAsByte<ETeamAttitude::Type> Attitude[EGameTeam::Num];	

	FTeamAttitude();

	FTeamAttitude(std::initializer_list<TEnumAsByte<ETeamAttitude::Type>> attitudes);
};