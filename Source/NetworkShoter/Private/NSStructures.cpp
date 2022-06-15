// Fill out your copyright notice in the Description page of Project Settings.


#include "NSStructures.h"



FPlayerStatistic& FPlayerStatistic::operator+=(const FPlayerStatistic& Other){

	this->AssistCount += Other.AssistCount;
	this->KillCount += Other.KillCount;
	this->DeathCount += Other.DeathCount;
	
	return *this;
}


FDamageInfo::FDamageInfo(AActor* InInstigator, AActor* InCauser, AActor* InTarget, float InDamage)
	:Instigator(InInstigator), DamageCauser(InCauser), Target(InTarget), Damage(InDamage)
{
	if (Target)
		Time = Target -> GetWorld() -> GetTimeSeconds();
}
