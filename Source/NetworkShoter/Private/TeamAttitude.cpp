// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamAttitude.h"

FTeamAttitude::FTeamAttitude()
{
	for (auto &Element : Attitude)
	{
		Element = ETeamAttitude::Neutral;
	}
}

FTeamAttitude::FTeamAttitude(std::initializer_list<TEnumAsByte<ETeamAttitude::Type>> attitudes)// : FTeamAttitude()
{
	int32 Count = 0;
	for (auto &Element : attitudes)
	{
		Attitude[Count] = Element;
		Count++;
	}
}
