// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/NSSettingsUIInfo.h"

UNSSettingsUIInfo* UNSSettingsUIInfo::Get()
{
	return GetMutableDefault<UNSSettingsUIInfo>();
}
