// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "NSGameMode.h"
#include "NSPlayerState.h"
#include "PCNetShooter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSGameState, Team1);
	DOREPLIFETIME(ANSGameState, Team2);
	DOREPLIFETIME(ANSGameState, MatchTimeLimit);
	DOREPLIFETIME(ANSGameState, MatchState);
}

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

void ANSGameState::CharacterKilled(APawn* WhoKilled)
{
	RemovePawn(WhoKilled);
	AddStatisticWhenPawnKilled(WhoKilled);
}


void ANSGameState::AddPlayerPawn(APawn* Pawn)
{
	//TODO Get team
	FName TeamName = "Team1";

	//Get Team Array
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(TeamName, TeamListPtr);
	if (!TeamListPtr) { return; }
	
	TeamListPtr -> AddUnique(Pawn);
}

void ANSGameState::RemovePawn(APawn* Pawn)
{
	//TODO Get team
	FName TeamName = "Team1";

	
	//remove pawn from team array
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(TeamName, TeamListPtr);
	
	if (TeamListPtr)
		TeamListPtr -> Remove(Pawn);
}

void ANSGameState::GetNextActorInTeam(FName Team, AActor*& NextActorInTeam, int32& NumberInTeam, bool bNext)
{
	//Get Team array
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(Team, TeamListPtr);
	if (!ensure(TeamListPtr)) { return; }

	//if Empty return failed result
	if (TeamListPtr -> Num() <= 0)
	{
		NextActorInTeam = nullptr;
		NumberInTeam = -1;
		return;
	}

	//if Previous actor or index was been set
	auto PreviousActorInTeam = NextActorInTeam;
	if (PreviousActorInTeam || (NumberInTeam>-1))
	{
		//Get his index in array
		int32 PreviousActorIndex = -1;
		if (PreviousActorInTeam)
		{
			for (int32 i = 0; i < TeamListPtr->Num(); i++)
			{	
				if ((*TeamListPtr)[i] == PreviousActorInTeam)
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
				
				NextActorInTeam = (*TeamListPtr)[NextIndex];
				NumberInTeam = NextIndex;
				return;
			}
			else
			{
				int32 PreviousIndex = (PreviousActorIndex-1 >= 0) ? PreviousActorIndex-1 : TeamListPtr->Num()-1;
				
				NextActorInTeam = (*TeamListPtr)[PreviousIndex];
				NumberInTeam = PreviousIndex;
				return;
			}
		}
	}

	//if not set or not find, return 0 element
	NextActorInTeam = (*TeamListPtr)[0];
	NumberInTeam = 0;
	return;
}

void ANSGameState::GetTeamList(FName Team, TArray<APawn*>*& TeamListPtr)
{
	if (Team == "Team1")
	{
		TeamListPtr = &Team1;
		return;
	}
	if (Team == "Team2")
	{
		TeamListPtr = &Team2;
		return;
	}
}

void ANSGameState::OnRep_Team1()
{
}

void ANSGameState::OnRep_Team2()
{
}

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

