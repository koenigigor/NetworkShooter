// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameMode.h"

#include "EngineUtils.h"
#include "NSPlayerStart.h"
#include "Game/NSPlayerState.h"
#include "Game/PCNetShooter.h"
#include "Game/NSGameState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

ANSGameMode::ANSGameMode()
{
	bStartPlayersAsSpectators = true;
}

void ANSGameMode::InitGameState()
{
	Super::InitGameState();

	NSGameState = Cast<ANSGameState>(GameState);
	if (NSGameState)
	{
		NSGameState->MatchTimeLimit = MatchTimeLimit;
		NSGameState->bMatchTimeLimit = bMatchTimeLimit;
		NSGameState->bFriendlyFire = bFriendlyFire;
		NSGameState->WaitStartMatchTime = WaitStartMatchTime;
	}
}

void ANSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//NewPlayer -> GetPlayerState<ANSPlayerState>() -> Team = "Team A";
}

//~==============================================================================================
// Change Match State

void ANSGameMode::StartMatch()
{
	if (HasMatchStarted())
	{
		UE_LOG(LogTemp, Warning, TEXT("Match already started"))
		return;
	}

	//Let the game session override the StartMatch function, in case it wants to wait for arbitration
	if (GameSession -> HandleStartMatchRequest()) { return; }
	
	SetMatchState(EMatchState::InProgress);
}

void ANSGameMode::EndMatch()
{
	if (!HasMatchStarted())
	{
		UE_LOG(LogTemp, Error, TEXT("Cant end not started match"))
		return;
	}
	
	SetMatchState(EMatchState::PostMatch);
}

bool ANSGameMode::HasMatchStarted() const
{
	//return Super::HasMatchStarted();
	return MatchState == EMatchState::InProgress;
}

void ANSGameMode::SetMatchState(EMatchState NewMatchState)
{
	if (MatchState == NewMatchState){ return; }

	MatchState = NewMatchState;
	
	if (NSGameState)
	{
		NSGameState->MatchState = MatchState;
	}

	if (MatchState == EMatchState::WaitingToStart)
    {
		
    	return;
    }

	if (MatchState == EMatchState::InProgress)
	{
		StartMatchHandle();
		return;
	}

	if (MatchState == EMatchState::PostMatch)
	{
		EndMatchHandle();
		return;
	}
}

void ANSGameMode::StartMatchHandle()
{
	UE_LOG(LogTemp, Error, TEXT("Start match"))
	
	BP_MatchStarted();
	if (NSGameState)
	{
		NSGameState->StartMatchHandle();
	}
}

void ANSGameMode::EndMatchHandle()
{
	UE_LOG(LogTemp, Error, TEXT("End match"))
	
	BP_MatchFinished();
	if (NSGameState)
	{
		NSGameState->EndMatchHandle();
	}
}


//~==============================================================================================
// Respawn player

void ANSGameMode::CharacterKilled(APawn* WhoKilled)
{
	if (WhoKilled->GetController()->IsPlayerController())
		DeathControllers.Add(StaticCast<APlayerController*>(WhoKilled->GetController()));

	GetGameState<ANSGameState>() -> CharacterKilled(WhoKilled);

	if (bRespawnAfterDeath)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ANSGameMode::RespawnDeathPlayer, RespawnDelay);
	}

	if (bLimitByTeamKills)
	{
		auto TeamIndex = WhoKilled->GetPlayerState<ANSPlayerState>()->TeamIndex;
		if (NSGameState && NSGameState -> GetTeamStatistic(TeamIndex).DeathCount >= LimitByTeamKills)
		{
			EndMatch();
		}
	}
		
}

void ANSGameMode::SpawnPlayer(AController* Controller)
{
	Controller->SetPawn(nullptr); //clear previous pawn, for not teleport it (spectator)
	Controller->StartSpot = nullptr; //clear start spot for get new spot
	
	RestartPlayer(Controller);

	//send respawn notify to player state
	if (auto NSPlayerState = Controller -> GetPlayerState<ANSPlayerState>())
	{
		NSPlayerState -> RespawnHandle();
	}
}

void ANSGameMode::RespawnDeathPlayer()
{
	if (HasMatchStarted() && DeathControllers.IsValidIndex(0))
	{
		SpawnPlayer(DeathControllers[0]);
		DeathControllers.RemoveAt(0);
	}
}

AActor* ANSGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	for (TActorIterator<ANSPlayerStart> It(World); It; ++It)
	{
		ANSPlayerStart* PlayerStart = *It;

		if (PlayerStart->CanSpawn(Player))
		{
			FVector ActorLocation = PlayerStart->GetActorLocation();
			const FRotator ActorRotation = PlayerStart->GetActorRotation();
			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
			{
				UnOccupiedStartPoints.Add(PlayerStart);
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				OccupiedStartPoints.Add(PlayerStart);
			}
		}
	}

	//if start not found
	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
			UE_LOG(LogTemp, Error, TEXT("ANSGameMode::ChoosePlayerStart free start not found"))
		}
	}
	
	return FoundPlayerStart;
}

