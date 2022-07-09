// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/ShootBase.h"
#include "ShootTrace.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UShootTrace : public UShootBase
{
	GENERATED_BODY()
protected:
	virtual void MakeSingleShoot() override;
};
