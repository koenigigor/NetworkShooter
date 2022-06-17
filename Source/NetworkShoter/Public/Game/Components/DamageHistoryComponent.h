// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSStructures.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DamageHistoryComponent.generated.h"



/** GameState component, keep match statistic, damage
 *	TeamKills, Assists etc */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UDamageHistoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageHistoryComponent();

public:
	/** return array players who damage target */
	TArray<APlayerState*> GetAssist(const AActor* Target);
 
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnDamage(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	//add statistic
	void OnDeath(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	//Target and damage info
	TMap<TObjectPtr<AActor>, TArray<FDamageInfo>> DamageHistory;

	FGameplayMessageListenerHandle DamageListener;
	FGameplayMessageListenerHandle DeathListener;
};
