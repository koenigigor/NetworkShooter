// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/NSAIPerceptionComponent.h"

#include "Game/NSPlayerState.h"

void UNSAIPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnTargetPerceptionUpdated.AddDynamic(this, &UNSAIPerceptionComponent::TargetPerceptionUpdated);

	//FIXME
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UNSAIPerceptionComponent::DetectLoseTarget, 1.f, true, 1.f);
}

void UNSAIPerceptionComponent::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
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
	if (!PriorityEnemy) return;
	
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

void UNSAIPerceptionComponent::DetectLoseTarget()
{
	const FActorPerceptionInfo* Info = GetActorInfo(*PriorityEnemy);
    				
	if (Info && Info->LastSensedStimuli.Num() > 0)
	{
		const FAIStimulus Stimulus = Info->LastSensedStimuli[0];
    
		if (Stimulus.WasSuccessfullySensed()) {
			//Player in sight
			return;
		}
	}
	
	OnLosePriorityEnemy();		
}
