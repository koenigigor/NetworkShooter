// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/NSAIPerceptionComponent.h"

#include "Game/NSPlayerState.h"

void UNSAIPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnTargetPerceptionUpdated.AddDynamic(this, &UNSAIPerceptionComponent::TargetPerceptionUpdated);
}

void UNSAIPerceptionComponent::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	//todo if lose actor
	
	if (!PriorityEnemy)
	{
		SetAsPriority(Actor);
	}
}

void UNSAIPerceptionComponent::SetAsPriority(AActor* Actor)
{
	if (!Actor) {return;}
	
	PriorityEnemy = Actor;
	PriorityEnemyUpdated.Broadcast(PriorityEnemy);

	//bind to target live
	PriorityEnemy->OnDestroyed.AddDynamic(this, &UNSAIPerceptionComponent::OnTargetDestroy);
	if (auto PriorityEnemyPawn = Cast<APawn>(PriorityEnemy))
	{
		if (auto NSPlayerState = PriorityEnemyPawn->GetPlayerState<ANSPlayerState>())
		{
			NSPlayerState->CharacterDeadDelegate.AddDynamic(this, &UNSAIPerceptionComponent::OnTargetDeath);
		}
	}
}


void UNSAIPerceptionComponent::ClearPriority()
{
	//unbind live
	PriorityEnemy->OnDestroyed.RemoveDynamic(this, &UNSAIPerceptionComponent::OnTargetDestroy);
	if (auto PriorityEnemyPawn = Cast<APawn>(PriorityEnemy))
	{
		if (auto NSPlayerState = PriorityEnemyPawn->GetPlayerState<ANSPlayerState>())
		{
			NSPlayerState->CharacterDeadDelegate.RemoveDynamic(this, &UNSAIPerceptionComponent::OnTargetDeath);
		}
	}
	
	PriorityEnemy = nullptr;
}

void UNSAIPerceptionComponent::OnLosePriorityEnemy()
{
	ClearPriority();
	
	//find new priority
	SetAsPriority(GetNewPriorityEnemy());
	
	PriorityEnemyUpdated.Broadcast(PriorityEnemy);
}

AActor* UNSAIPerceptionComponent::GetNewPriorityEnemy()
{
	//FIXME
	//get random enemy
	TArray<AActor*> OutActors;
	GetKnownPerceivedActors(nullptr, OutActors);
	
	if (OutActors.Num() == 0) return nullptr;

	return  OutActors[FMath::RandRange(0, OutActors.Num()-1)];
}



void UNSAIPerceptionComponent::OnTargetDestroy(AActor* DestroyedActor)
{
	OnLosePriorityEnemy();
}

void UNSAIPerceptionComponent::OnTargetDeath(APawn* WhoDeath)
{
	OnLosePriorityEnemy();
}
