// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAttitude.generated.h"

/**
 * find there
 * https://forums.unrealengine.com/t/how-do-i-use-the-ai-perception-teams/120837/2
 */

UENUM()
enum class EGameTeam : uint8
{
	Neutral,
	Team1,
	Team2
};

USTRUCT(BlueprintType)
struct FTeamAttitude
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<ETeamAttitude::Type>> Attitude;

	FTeamAttitude() {};

	FTeamAttitude(std::initializer_list<TEnumAsByte<ETeamAttitude::Type>> attitudes)
		:Attitude(std::move(attitudes)){};
};
