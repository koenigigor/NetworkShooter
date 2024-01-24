// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamAttitudeSettings.h"

UTeamAttitudeSettings::UTeamAttitudeSettings(const FObjectInitializer& ObjectInitializer)
{
/*
	typedef ETeamAttitude::Type EA;
	TeamAttitudes = {
		{EA::Friendly, EA::Hostile,  EA::Hostile},//Neutral
		{EA::Hostile, EA::Friendly, EA::Hostile},//Team1
		{EA::Hostile, EA::Hostile,  EA::Friendly},//Team2
	};
	*/
}

const UTeamAttitudeSettings* UTeamAttitudeSettings::Get()
{
	return GetDefault<UTeamAttitudeSettings>();
}

ETeamAttitude::Type UTeamAttitudeSettings::GetAttitude(FGenericTeamId Of, FGenericTeamId Towards)
{
	auto& teamAttitudes = Get()->TeamAttitudes;
	
	auto AttitudesNum = (uint8)EGameTeam::Num;
	
	bool ofValid = Of.GetId() < AttitudesNum;
	bool towardsValid = Towards.GetId() < AttitudesNum;

	if (ofValid && towardsValid)
	{
		auto& attitudes = teamAttitudes[Of.GetId()].Attitude;
		if (Towards.GetId() < AttitudesNum)
		{
			return attitudes[Towards.GetId()];
		}
	}
	
	return ETeamAttitude::Neutral;
}
