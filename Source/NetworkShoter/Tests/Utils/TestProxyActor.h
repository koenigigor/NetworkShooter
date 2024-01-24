// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestProxyActor.generated.h"

class UGameplayEffect;

USTRUCT()
struct FCharacterTest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> HalfHpDamage = nullptr;
};

/** cant load blueprint gameplay effect, so be save it in actor,
 *	and spawn actor
 *	 Yes, i can make instant GE, how it make in samples,
 *	 but i wanna can use blueprint gameplay effects,
 *	 *and other classes
 */
UCLASS()
class NETWORKSHOTER_API ATestProxyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestProxyActor();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageKillEffect = nullptr;

	UPROPERTY(EditAnywhere)
	FCharacterTest CharacterTestData;
};
