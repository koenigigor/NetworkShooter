// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "NSGameSession.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameSession : public AGameSession
{
	GENERATED_BODY()
	
	virtual void RegisterServer() override;
};
