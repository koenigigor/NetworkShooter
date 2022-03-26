// Fill out your copyright notice in the Description page of Project Settings.


#include "NSStructures.h"



FPlayerStatistic& FPlayerStatistic::operator+=(const FPlayerStatistic& Other){

	this->AssistCount += Other.AssistCount;
	this->KillCount += Other.KillCount;
	this->DeathCount += Other.DeathCount;
	
	return *this;
}
