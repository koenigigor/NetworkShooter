// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/TeamSetupManager.h"

#include "Game/NSGameState.h"
#include "Game/NSPlayerState.h"
#include "Net/UnrealNetwork.h"



UTeamSetupManager::UTeamSetupManager()
{
	PrimaryComponentTick.bCanEverTick = false; //timers need tick ???
	
}

void UTeamSetupManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTeamSetupManager, Teams, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UTeamSetupManager, ChooseTeamDelay, COND_InitialOnly);
}

void UTeamSetupManager::ShuffleTeam(bool AddAlreadyInTeam)
{
	const auto CountOfTeams = Teams.Num();
	if (CountOfTeams == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTeamSetupManager::ShuffleTeam, Cant shaffle Teams not setup"))
		return;
	}
	
	const auto PlayersCount = GetWorld() -> GetGameState() -> PlayerArray.Num();
	int32 PlayersPerTeam = PlayersCount / CountOfTeams;
	if (PlayersCount % CountOfTeams != 0)
		PlayersPerTeam++;
	
	TArray<ANSPlayerState*> PlayersToShuffle;
	GetPlayersToShuffle(PlayersToShuffle, AddAlreadyInTeam);
	
	TMap<uint8, int32> TeamCountMap;
	GetTeamAndCount(TeamCountMap, AddAlreadyInTeam);

	//shuffle teams
	for (auto Player : PlayersToShuffle) 
	{
		//GetRandom team id, if no search again
		uint8 TeamToAdd = uint8(Teams[FMath::RandRange(0, Teams.Num()-1)]);
		if (TeamCountMap[TeamToAdd] >= PlayersPerTeam)
		{
			//get team with free slots
			TArray<uint8> FreeTeams;
			for (const auto& Team : Teams)
			{
				if (TeamCountMap[uint8(Team)] < PlayersPerTeam)
				{
					FreeTeams.Add(uint8(Team));
				}
			}

			if (FreeTeams.Num()==0)
			{
				UE_LOG(LogTemp, Warning, TEXT("UTeamSetupManager::AddPlayerInTeam Free Team Not Found"))
				return;
			}

			//get random of them
			TeamToAdd = FreeTeams[FMath::RandRange(0, FreeTeams.Num()-1)];
		}
		
		TeamCountMap[TeamToAdd]++;
		
		Player->SetGenericTeamId(FGenericTeamId(TeamToAdd));
	}
}

void UTeamSetupManager::AddPlayerInTeam_Implementation(ANSPlayerState* Player, EGameTeam PreferTeam)
{
	ClearIfWaitingChoose(Player);

	const auto CountOfTeams = Teams.Num();
	if (CountOfTeams == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("UTeamSetupManager::AddPlayerInTeam, Cant shaffle Teams not setup"))
		return;
	}

	if (!Teams.Contains(PreferTeam))
		PreferTeam = Teams[0];
	
	const auto PlayersCount = GetWorld()->GetGameState() -> PlayerArray.Num();
	int32 PlayersPerTeam = PlayersCount / CountOfTeams;
	if (PlayersCount % CountOfTeams != 0)
		PlayersPerTeam++;

	TMap<uint8, int32> TeamCountMap;
	GetTeamAndCount(TeamCountMap, true);
	
	uint8 TeamToAdd = uint8(PreferTeam);

	if (TeamCountMap[TeamToAdd] >= PlayersPerTeam)
	{
		//get team with free slots
		TArray<uint8> FreeTeams;
		for (const auto& Team : Teams)
		{
			if (TeamCountMap[uint8(Team)] < PlayersPerTeam)
			{
				FreeTeams.Add(uint8(Team));
			}
		}

		if (FreeTeams.Num()==0)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTeamSetupManager::AddPlayerInTeam Free Team Not Found"))
			return;
		}

		//get random of them
		TeamToAdd = FreeTeams[FMath::RandRange(0, FreeTeams.Num()-1)];
	}
	
	Player->SetGenericTeamId(FGenericTeamId(TeamToAdd));
}

bool UTeamSetupManager::AddPlayerInTeam_Validate(ANSPlayerState* Player, EGameTeam PreferTeam)
{
	return true;
}

void UTeamSetupManager::WaitUntilPlayerChooseTeam(APlayerController* Player)
{
	auto PlayerState = Player->GetPlayerState<ANSPlayerState>();

	if (!PlayerState)
	{
		UE_LOG(LogTemp, Log, TEXT("UTeamSetupManager::WaitUntilPlayerChooseTeam PlayerState is not a ANSPlayerState"))
	}
	
	FTimerHandle WaitTimerHandle;
	WaitChooseTeamTimers.Add(PlayerState, WaitTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this, &UTeamSetupManager::OnChooseTimeEnd, ChooseTeamDelay);
}

void UTeamSetupManager::OnChooseTimeEnd()
{
	TArray<ANSPlayerState*> EndWaiting;
	//get player who ends time
	for (const auto& Waiting : WaitChooseTeamTimers)
	{
		float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(Waiting.Value);
		if (RemainingTime <= 0)
		{
			EndWaiting.Add(Waiting.Key);
		}
	}
	
	//add players in random team
	for (const auto& Player : EndWaiting)
	{
		AddPlayerInTeam(Player, Teams[FMath::RandRange(0, Teams.Num()-1)]);
	} 
}

void UTeamSetupManager::ClearIfWaitingChoose(ANSPlayerState* Player)
{
	if (!WaitChooseTeamTimers.Contains(Player)) return;

	auto TimerHandel = WaitChooseTeamTimers.FindAndRemoveChecked(Player);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandel);
	
	if (auto Controller = Cast<APlayerController>(Player->GetOwner()))
		PlayerChooseTeamAfterWait.Broadcast(Controller);
}

TArray<ANSPlayerState*> UTeamSetupManager::GetTeam(uint8 TeamIndex)
{
	TArray<ANSPlayerState*> Output;
	
	for (auto& Player : GetWorld()->GetGameState()->PlayerArray)
	{
		auto NSPlayer = StaticCast<ANSPlayerState*>(Player);
		if (NSPlayer->GetGenericTeamId().GetId() == TeamIndex)
			Output.Add(NSPlayer);
	}

	return Output;
}

void UTeamSetupManager::GetTeamAndCount(TMap<uint8, int32>& TeamCountMap, const bool CheckIfAlreadyInTeam)
{
	TeamCountMap.Empty();

	//create keys for used teams, for protect using this map outside
	for (auto& Team : Teams)
	{
		TeamCountMap.Add(uint8(Team), 0);
	} 

	if (!CheckIfAlreadyInTeam) { return; }
	
	auto PlayerArray = GetWorld()->GetGameState()->PlayerArray;
	for (const auto& Player : PlayerArray)
	{
		auto TeamId = Cast<IGenericTeamAgentInterface>(Player)->GetGenericTeamId().GetId();
		if (TeamCountMap.Contains(TeamId))
		{
			TeamCountMap[TeamId]++;
		}
		else
		{
			TeamCountMap.Add(TeamId, 1);
		}
	} 
}

void UTeamSetupManager::GetPlayersToShuffle(TArray<ANSPlayerState*>& PlayersToShuffle, bool AddAlreadyInTeam)
{
	PlayersToShuffle.Empty();
	
	auto PlayerArray = GetWorld()->GetGameState() -> PlayerArray;
	
	for (auto& Player : PlayerArray)
	{
		auto NSPlayer = Cast<ANSPlayerState>(Player);
		if (AddAlreadyInTeam)
		{
			PlayersToShuffle.Add(NSPlayer);
		}
		else
		{
			if (!Teams.Contains(NSPlayer->GetTeamID_Verbose()))
			{
				PlayersToShuffle.Add(NSPlayer);
			}
		}
	} 
}





