// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamAttitudeSettings.h"

UTeamAttitudeSettings::UTeamAttitudeSettings(const FObjectInitializer& ObjectInitializer)
{
	typedef ETeamAttitude::Type EA;
	TeamAttitudes = {
		{EA::Friendly, EA::Hostile,  EA::Hostile},//Neutral
		{EA::Hostile, EA::Friendly, EA::Hostile},//Team1
		{EA::Hostile, EA::Hostile,  EA::Friendly},//Team2
	};
	
		/*{
	{EA::Friendly, EA::Neutral,  EA::Neutral,  EA::Neutral,  EA::Neutral },//Neutral
	{EA::Neutral, EA::Friendly, EA::Hostile,  EA::Friendly, EA::Hostile},//Team1
	{EA::Neutral, EA::Hostile,  EA::Friendly, EA::Hostile,  EA::Hostile},//Team2
	{EA::Neutral, EA::Friendly, EA::Hostile,  EA::Friendly, EA::Friendly },//Team3
	{EA::Neutral, EA::Hostile,  EA::Hostile,  EA::Friendly, EA::Friendly }//Team4
};*/
}

const UTeamAttitudeSettings* UTeamAttitudeSettings::Get()
{
	return GetDefault<UTeamAttitudeSettings>();
}

ETeamAttitude::Type UTeamAttitudeSettings::GetAttitude(FGenericTeamId Of, FGenericTeamId Towards)
{
	auto& teamAttitudes = Get()->TeamAttitudes;
	auto& teamAttitudes_TestV2 = Get()->TeamAttitudesV2; //todo for test 2
	
	auto AttitudesNum_TestV2 = (uint8)EGameTeam::Num;
	
	bool ofValid_Test2 = Of.GetId() < AttitudesNum_TestV2;
	bool towardsValid_Test2 = Towards.GetId() < AttitudesNum_TestV2;
	
	bool ofValid = teamAttitudes.IsValidIndex(Of.GetId());
	bool towardsValid = teamAttitudes.IsValidIndex(Towards.GetId());

	if (ofValid && towardsValid)
	{
		auto& attitudes = teamAttitudes[Of.GetId()].Attitude;
		if (attitudes.IsValidIndex(Towards.GetId()))
		{
			return attitudes[Towards.GetId()];
		}
	}

	if (ofValid_Test2 && towardsValid_Test2)
	{
		auto& attitudes = teamAttitudes_TestV2[Of.GetId()].Attitude_TestV2;
		if (Towards.GetId() < AttitudesNum_TestV2)
		{
			//return attitudes[Towards.GetId()];
		}
	}
	
	return ETeamAttitude::Neutral;
}
