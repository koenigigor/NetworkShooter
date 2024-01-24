// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "NSAIPerceptionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPriorityEnemyUpdated, AActor*, NewEnemy);
/** Can*t send nullptr from EnemyUpdate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPriorityEnemyClear); 

/**
 * Subclass of AIPerception component for network shooter
 * with ability select priority enemy
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UNSAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	void SetAsPriority(AActor* Actor);

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	void ClearPriorityEnemyBindings();

	/** called when priority enemy Forget, or death, or etc. */
	void OnLosePriorityEnemy();

	AActor* GetNewPriorityEnemy();

	UPROPERTY()
	AActor* PriorityEnemy = nullptr;
	
	UPROPERTY(BlueprintAssignable)
	FPriorityEnemyUpdated PriorityEnemyUpdated;

	UPROPERTY(BlueprintAssignable)
	FPriorityEnemyClear PriorityEnemyClear;

	UFUNCTION()
	void OnTargetDestroy(AActor* DestroyedActor);
	UFUNCTION()
	void OnTargetDeath(APawn* WhoDeath);

	/** FIXME temporary function until find good solution for detect when Actor has get of sence */
	UFUNCTION()
	void DetectLoseTarget();
};
