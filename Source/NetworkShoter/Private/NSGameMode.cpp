// Fill out your copyright notice in the Description page of Project Settings.


#include "NSGameMode.h"

#include "EngineUtils.h"
#include "NSPlayerStart.h"
#include "PCNetShooter.h"
#include "Game/NSGameState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

void ANSGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerDeath.AddDynamic(this, &ANSGameMode::CharacterKilled);
}

void ANSGameMode::CharacterKilled(APawn* WhoKilled, AController* InstigatedBy, AActor* DamageCauser)
{
	//do something

	UE_LOG(LogTemp, Warning, TEXT("GameMode say: %s is died"), *WhoKilled->GetName())
	
	if (WhoKilled->GetController()->IsPlayerController())
		DeathControllers.Add(StaticCast<APlayerController*>(WhoKilled->GetController()));

	GetGameState<ANSGameState>() -> RemovePawn(WhoKilled);
}

void ANSGameMode::SpawnPlayer(APlayerController* Controller)
{
	//just restart player can teleport current possessed pawns to start point,
	//here need spawn a new pawn
	Controller->SetPawn(nullptr); //clear previous pawn (spectator)
	
	RestartPlayer(Controller);

	//add pawn in team list
	if (auto NSGameState = GetGameState<ANSGameState>())
	{
		NSGameState -> AddPlayerPawn(Controller->GetPawn());
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
		}

		UE_LOG(LogTemp, Error, TEXT("ANSGameMode::ChoosePlayerStart valid start not found"))
	}
	
	return FoundPlayerStart;
}

