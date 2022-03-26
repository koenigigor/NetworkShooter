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
}


//~==============================================================================================
// Match State

void ANSGameState::StartMatchHandle_Implementation()
{	
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

void ANSGameState::GetNextPlayerInTeam(int32 TeamIndex, ANSPlayerState*& NextPlayerInTeam, int32& NumberInTeam, bool bNext)
{
	//TODO
	return;
	
	//Get Team array
	TArray<ANSPlayerState*>* TeamListPtr = nullptr;
	//GetTeamList(TeamIndex, TeamListPtr);
	if (!ensure(TeamListPtr)) { return; }

	//if Empty return failed result
	if (TeamListPtr -> Num() <= 0)
	{
		NextPlayerInTeam = nullptr;
		NumberInTeam = -1;
		return;
	}

	//if Previous actor or index was been set
	auto PreviousPlayerInTeam = NextPlayerInTeam;
	if (PreviousPlayerInTeam || (NumberInTeam>-1))
	{
		//Get his index in array
		int32 PreviousActorIndex = -1;
		if (PreviousPlayerInTeam)
		{
			for (int32 i = 0; i < TeamListPtr->Num(); i++)
			{	
				if ((*TeamListPtr)[i] == PreviousPlayerInTeam)
				{
					PreviousActorIndex = i;
					break;
				}
			}
		}
		
		//if previous actor not founded, check index
		if (PreviousActorIndex == -1)
		{
			PreviousActorIndex = TeamListPtr->IsValidIndex(NumberInTeam) ? NumberInTeam : -1;
			//todo if dead character give self number, maybe need do -1, and protect 0 -> -1
		}

		
		//if index successfully founded founded
		if (PreviousActorIndex > -1)
		{
			//return next or previous element
			if (bNext)
			{
				int32 NextIndex = (PreviousActorIndex+1 < TeamListPtr->Num()) ? PreviousActorIndex+1 : 0;
				
				PreviousPlayerInTeam = (*TeamListPtr)[NextIndex];
				NumberInTeam = NextIndex;
				return;
			}
			else
			{
				int32 PreviousIndex = (PreviousActorIndex-1 >= 0) ? PreviousActorIndex-1 : TeamListPtr->Num()-1;
				
				PreviousPlayerInTeam = (*TeamListPtr)[PreviousIndex];
				NumberInTeam = PreviousIndex;
				return;
			}
		}
	}

	//if not set or not find, return 0 element
	PreviousPlayerInTeam = (*TeamListPtr)[0];
	NumberInTeam = 0;
	return;
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

