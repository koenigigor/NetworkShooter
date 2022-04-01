// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Game/NSGameMode.h"
#include "Game/NSPlayerState.h"
#include "Game/PCNetShooter.h"
#include "GameFramework/PlayerState.h"
#include "Items/Weapon.h"
#include "Net/UnrealNetwork.h"


ANSGameState::ANSGameState()
{
	SetActorTickEnabled(true);
	SetActorTickInterval(1.f);
}

void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANSGameState, MatchTimeLimit);
	DOREPLIFETIME(ANSGameState, MatchState);
	DOREPLIFETIME(ANSGameState, bMatchTimeLimit);
	DOREPLIFETIME(ANSGameState, bFriendlyFire);
	DOREPLIFETIME(ANSGameState, MatchTime);
	DOREPLIFETIME(ANSGameState, DamageInfoList);
	DOREPLIFETIME_CONDITION(ANSGameState, WaitStartMatchTime, COND_InitialOnly);
}

void ANSGameState::BeginPlay()
{
	Super::BeginPlay();

	if (GetMatchState() == EMatchState::WaitingConnection || GetMatchState() == EMatchState::WaitingToStart)
		MatchTime = WaitStartMatchTime;
}

void ANSGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	if (MatchState == EMatchState::WaitingToStart)
	{
		MatchTime--;
		
		if (MatchTime<=0 && GetWorld()->IsServer())
		{
			Cast<ANSGameMode>(GetWorld()->GetAuthGameMode())->StartMatch();
		}
	} else
	if (MatchState == EMatchState::InProgress)
	{
		MatchTime++;
		
		if (bMatchTimeLimit && GetWorld()->IsServer() && MatchTime >= MatchTimeLimit.GetTotalSeconds())
		{
			Cast<ANSGameMode>(GetWorld()->GetAuthGameMode())->EndMatch();
		}
	}
}

void ANSGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	PlayerAddedDelegate.Broadcast(PlayerState);
}

void ANSGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	PlayerRemovedDelegate.Broadcast(PlayerState);
}


//~==============================================================================================
// Match State

void ANSGameState::WaitingToStartMatchHandle_Implementation()
{
	MatchTime = WaitStartMatchTime;
	WaitingToStartMatch.Broadcast();
}

void ANSGameState::StartMatchHandle_Implementation()
{
	BP_MatchStarted();
	MatchStartDelegate.Broadcast();
	
	MatchStartTime = GetWorld()->GetTimeSeconds();
	MatchTime = 0.f;
}

void ANSGameState::EndMatchHandle_Implementation()
{	
	BP_MatchFinished();
	MatchEndDelegate.Broadcast();
}

void ANSGameState::CharacterKilled(APawn* WhoKilled)
{
	AddStatisticWhenPawnKilled(WhoKilled);
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
	
	//temp prototype
	auto PCNetShooter = Cast<APCNetShooter>(DamageInfo.DamagedActor->GetInstigatorController());
	if (PCNetShooter)
	{
		PCNetShooter->NotifyReceiveDamage_Implementation(DamageInfo.Damage, FVector::ZeroVector, FName(DamageInfo.InstigatorName), DamageInfo.DamageCauser);
	}
}

bool ANSGameState::CanDamage(AActor* DamagedActor, AActor* DamageCauser)
{
	if (bFriendlyFire)
	{
		//todo get team id interface
		if (DamagedActor->GetInstigator()->GetPlayerState<ANSPlayerState>())
		{
			if (DamageCauser->GetInstigator()->GetPlayerState<ANSPlayerState>())
			{
				auto TeamDamagedActor = DamagedActor->GetInstigator()->GetPlayerState<ANSPlayerState>()->TeamIndex;
				auto TeamDamageCauser = DamageCauser->GetInstigator()->GetPlayerState<ANSPlayerState>()->TeamIndex;
				if (TeamDamagedActor == TeamDamageCauser && TeamDamagedActor!=-1)
				{
					UE_LOG(LogTemp, Warning, TEXT("CanDamage: Cant damage your timmates"))
					return false;
				}
				UE_LOG(LogTemp, Warning, TEXT("CanDamage: Team not equal, damage accept"))
			}
			UE_LOG(LogTemp, Warning, TEXT("CanDamage: DamageCauser player state not fount"))
		}
		UE_LOG(LogTemp, Warning, TEXT("CanDamage: DamagedActor player state not fount"))
	}
	UE_LOG(LogTemp, Warning, TEXT("CanDamage: Damage accept"))
	return true;
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
	
	if (DamageCauser)
	{
		if (auto Weapon = Cast<AWeapon>(DamageCauser))
		{
			CauserName = Weapon -> WeaponData -> Name.ToString();
		}
		else
		{
			CauserName = DamageCauser->GetName();
		}
	}
	else
	{
		CauserName = "No WeaponName";
	}
	
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

FDamageInfo ANSGameState::GetKillInfo(APawn* WhoKilled)
{
	FDamageInfo DamageInfo;
	for (auto i = DamageInfoList.Num()-1; i>0; i--)
	{
		if (DamageInfoList[i].DamagedActor == WhoKilled)
		{
			DamageInfo = DamageInfoList[i];
			return DamageInfo;
		}
	}
	return DamageInfo;
}

void ANSGameState::AddStatisticWhenPawnKilled(APawn* WhoKilled)
{
	if (DamageInfoList.Num() == 0) { return; } 
	
	//get last damage info for this pawn
	FDamageInfo DamageInfo = GetKillInfo(WhoKilled);

	int32 DeathActorTeam = WhoKilled->GetPlayerState<ANSPlayerState>()->TeamIndex;
	
	//add kill count
    if (DamageInfo.Instigator)
    {
		int32 InstigatorTeam = DamageInfo.Instigator -> GetPlayerState<ANSPlayerState>() ->TeamIndex;

    	//if same team, decrease
    	if (DeathActorTeam == -1 || DeathActorTeam != InstigatorTeam)
    	{
    		DamageInfo.Instigator -> GetPlayerState<ANSPlayerState>() -> AddKill();
    	}
        else
        {
        	DamageInfo.Instigator -> GetPlayerState<ANSPlayerState>() -> AddKill(-1);
        }
    }
    
    //add death count
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
			int32 AssistantTeam = Assistant -> GetPlayerState<ANSPlayerState>() ->TeamIndex;

			if (DeathActorTeam == -1 || DeathActorTeam != AssistantTeam)
			{
				Assistant->GetPlayerState<ANSPlayerState>() -> AddAssist();
			}
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

float ANSGameState::GetMatchTimerRemaining()
{
	if (MatchState==EMatchState::InProgress)
	{
		return MatchTimeLimit.GetTotalSeconds() - MatchTime;
	}
	
	return -1.f;
}

