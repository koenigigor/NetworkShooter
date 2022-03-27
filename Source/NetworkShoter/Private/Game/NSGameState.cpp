// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Game/NSGameMode.h"
#include "Game/NSPlayerState.h"
#include "Game/PCNetShooter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANSGameState, MatchTimeLimit);
	DOREPLIFETIME(ANSGameState, MatchState);
	DOREPLIFETIME(ANSGameState, bMatchTimeLimit);
}


//~==============================================================================================
// Match State

void ANSGameState::StartMatchHandle_Implementation()
{
	if (bMatchTimeLimit)
		StartMatchTimer();
	
	BP_MatchStarted();
	MatchStartDelegate.Broadcast();
	
	MatchStartTime = GetWorld()->GetTimeSeconds();
}

void ANSGameState::EndMatchHandle_Implementation()
{	
	MatchTimerHandle.Invalidate();
	
	BP_MatchFinished();
	MatchEndDelegate.Broadcast();
}

void ANSGameState::CharacterKilled(APawn* WhoKilled)
{
	//RemovePawn(WhoKilled); //todo
	AddStatisticWhenPawnKilled(WhoKilled);
}

EMatchState ANSGameState::GetMatchState()
{
	return MatchState;
}

bool ANSGameState::HasMatchStarted() const
{
	//return Super::HasMatchStarted();
	return MatchState == EMatchState::InProgress;
}


//~==============================================================================================
// Match Statistic
void ANSGameState::ApplyDamageInfo(FDamageInfo DamageInfo)
{
	//if (!DamageInfo.IsValid) { return; }

	//add info in list
	DamageInfoList.Add(DamageInfo);

	UE_LOG(LogTemp, Warning, TEXT("%s deal %f damage to %s from %s"), *DamageInfo.InstigatorName, DamageInfo.Damage, *DamageInfo.DamagedActorName, *DamageInfo.DamageCauserName);


	
	//temp prototype
	auto PCNetShooter = Cast<APCNetShooter>(DamageInfo.DamagedActor->GetInstigatorController());
	if (PCNetShooter)
	{
		PCNetShooter->NotifyReceiveDamage_Implementation(DamageInfo.Damage, FVector::ZeroVector, FName(DamageInfo.InstigatorName), DamageInfo.DamageCauser);
	}
}

void ANSGameState::ApplyDamageInfoFromActors(AController* DamageInstigator, AActor* DamagedActor, AActor* DamageCauser, float Damage)
{
	FDamageInfo DamageInfo;
	FString InstigatorName;
	FString DamagedActorName;
	FString CauserName;
	
	//Prepare DamageInstigator name
	if (DamageInstigator)
	{
		if (DamageInstigator -> GetPlayerState<APlayerState>())
		{
			InstigatorName = DamageInstigator -> GetPlayerState<APlayerState>() -> GetPlayerName();
		}
		else
		{
			InstigatorName = DamageInstigator -> GetName();
		}
	}

	//Prepare DamageReceiver name
	if (DamagedActor->GetInstigatorController())
	{
		if (DamagedActor->GetInstigatorController()->GetPlayerState<APlayerState>())
		{
			DamagedActorName = DamagedActor->GetInstigatorController()->GetPlayerState<APlayerState>()->GetPlayerName();
		}
		else
		{
			DamagedActorName = DamagedActor->GetInstigatorController()->GetName();
		}
	}
	else
	{
		DamagedActorName = DamagedActor->GetName();
	}

	//Prepare CauserName
	//DamageInstigator->FindComponentByClass<ANSEquipnent>() -> GetEquippedWeapon() -> WeaponData -> Name;
	CauserName = "TODO WeaponName";
//TODO
	
	
	DamageInfo.Damage = Damage;

	DamageInfo.Instigator = DamageInstigator;
	DamageInfo.InstigatorName = InstigatorName;

	DamageInfo.DamagedActor = DamagedActor;
	DamageInfo.DamagedActorName = DamagedActorName;
	
	DamageInfo.DamageCauser = DamageCauser;
	DamageInfo.DamageCauserName = CauserName;
	
	DamageInfo.Time = GetWorld() -> GetTimeSeconds();
	
	ApplyDamageInfo(DamageInfo);
}

TArray<AController*> ANSGameState::GetAssist(AActor* DamagedActor)
{
	TArray<AController*> Assists;
	for (const auto& Info : DamageInfoList)
	{
		if (Info.DamagedActor && Info.DamagedActor == DamagedActor && Info.Instigator)
		{
			Assists.AddUnique(Info.Instigator);
		}
	}
	return Assists;
}

void ANSGameState::AddStatisticWhenPawnKilled(APawn* WhoKilled)
{
	if (DamageInfoList.Num() == 0) { return; } 
	
	//get last damage info for this pawn
	FDamageInfo DamageInfo;
	for (auto i = DamageInfoList.Num()-1; i>0; i--)
	{
		if (DamageInfoList[i].DamagedActor == WhoKilled)
		{
			DamageInfo = DamageInfoList[i];
			break;
		}
	}
	
	//add death count
    if (DamageInfo.Instigator)
    {
    	DamageInfo.Instigator -> GetPlayerState<ANSPlayerState>() -> AddKill();
    }
    
    //add kill count
    if (DamageInfo.DamagedActor && DamageInfo.DamagedActor->GetInstigatorController())
    {
    	DamageInfo.DamagedActor->GetInstigatorController() -> GetPlayerState<ANSPlayerState>() -> AddDeath();
    }
        		
	//add assist count
    auto AssistList = GetAssist(DamageInfo.DamagedActor);
    for (const auto& Assistant : AssistList)
	{
		if (Assistant != DamageInfo.Instigator)
		{
			Assistant->GetPlayerState<ANSPlayerState>() -> AddAssist();
        }
    }			
}


//~==============================================================================================
// Team List

TArray<ANSPlayerState*> ANSGameState::GetTeam(int32 TeamIndex)
{
	TArray<ANSPlayerState*> Output;
	
	for (auto Player : PlayerArray)
	{
		auto NSPlayer = StaticCast<ANSPlayerState*>(Player);
		if (NSPlayer->TeamIndex == TeamIndex)
			Output.Add(NSPlayer);
	}

	return Output;
}

int32 ANSGameState::GetTeamKills(int32 TeamId)
{
	return GetTeamStatistic(TeamId).KillCount;
}

FPlayerStatistic ANSGameState::GetTeamStatistic(int32 TeamId)
{
	FPlayerStatistic Statistic;

	auto Team = GetTeam(TeamId);
	for (const auto& Player : Team)
	{
		Statistic += Player->GetPlayerStatistic();
	}
	
	return Statistic;
}

void ANSGameState::GetNextPlayerInTeam(int32 TeamIndex, ANSPlayerState*& NextPlayerInTeam, bool bNext, bool bLifePlayer)
{
	auto Team = GetTeam(TeamIndex);

	//if Empty return failed result
	if (Team.Num() <= 0)
	{
		NextPlayerInTeam = nullptr;
		return;
	}

	//if Previous actor not set
	if (!NextPlayerInTeam)
	{
		//get first actor in team, or first life actor if need
		for (const auto& Player : Team)
		{
			if (!bLifePlayer || Player->IsLife())
			{
				NextPlayerInTeam = Player;
				return;
			}
		}
		NextPlayerInTeam = nullptr;
		return;
	}
	
	
	auto PreviousPlayerInTeam = NextPlayerInTeam;
	//Get his index in array
	int32 PreviousActorIndex = -1;
	if (PreviousPlayerInTeam)
	{
		for (int32 i = 0; i < Team.Num(); i++)
		{	
			if (Team[i] == PreviousPlayerInTeam)
			{
				PreviousActorIndex = i;
				break;
			}
		}
	}

	//if previous actor not found
	if (PreviousActorIndex == -1)
	{
		//get first actor in team, or first life actor if need
		for (const auto& Player : Team)
		{
			if (!bLifePlayer || Player->IsLife())
			{
				NextPlayerInTeam = Player;
				return;
			}
		}
		NextPlayerInTeam = nullptr;
		return;
	}

	
	//if index successfully founded founded
	
	//return next or previous element, life if need
	if (bNext)
	{
		int32 NextIndex = (PreviousActorIndex+1 < Team.Num()) ? PreviousActorIndex+1 : 0;
		while (NextIndex != PreviousActorIndex)
		{
			if (!bLifePlayer || Team[NextIndex]->IsLife())
			{
				NextPlayerInTeam = Team[NextIndex];
				return;
			}
			NextIndex = (NextIndex+1 < Team.Num()) ? NextIndex+1 : 0;
		}

		NextPlayerInTeam = nullptr;
		return;
	}
	else
	{
		int32 PreviousIndex = (PreviousActorIndex-1 >= 0) ? PreviousActorIndex-1 : Team.Num()-1;
		
		while (PreviousIndex != PreviousActorIndex)
		{
			if (!bLifePlayer || Team[PreviousIndex]->IsLife())
			{
				NextPlayerInTeam = Team[PreviousIndex];
				return;
			}
			PreviousIndex = (PreviousIndex-1 >= 0) ? PreviousIndex-1 : Team.Num()-1;
		}

		NextPlayerInTeam = nullptr;
		return;
	}
}




//~==============================================================================================
// Match timer
void ANSGameState::StartMatchTimer()
{
	GetWorld()->GetTimerManager().SetTimer(MatchTimerHandle, this, &ANSGameState::MatchTimerEnd, MatchTimeLimit.GetTotalSeconds());
}

void ANSGameState::MatchTimerEnd()
{
	//if it server, end match
	if (GetWorld()->IsServer())
	{
		auto NSGameMode = Cast<ANSGameMode>(GetWorld()->GetAuthGameMode());
		NSGameMode -> EndMatch();
	}
}

float ANSGameState::GetMatchTimerRemaining()
{
	if (MatchTimerHandle.IsValid())
	{
		return GetWorld()->GetTimerManager().GetTimerRemaining(MatchTimerHandle);
	}
	return -1.f;
}

