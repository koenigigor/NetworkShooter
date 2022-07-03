// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "NSInputAction.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSInputAction : public UInputAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, meta=(Categories="Input"))
	FGameplayTag InputTag;
};
