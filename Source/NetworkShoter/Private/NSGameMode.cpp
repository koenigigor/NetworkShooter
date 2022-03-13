// Fill out your copyright notice in the Description page of Project Settings.


#include "NSGameMode.h"

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
}

TArray<ANSPlayerStart*> ANSGameMode::GetFreePlayerStarts(FName CommandName)
{
	TArray<ANSPlayerStart*> FreePoints;

	//Get All Actors of class
	
	return FreePoints;
}

APawn* ANSGameMode::SpawnPlayer(APlayerController* Controller)
{
	//Get Player Start
	ANSPlayerStart* ResultPlayerStart;
	TArray<ANSPlayerStart*> ValidPlayerStarts;
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANSPlayerStart::StaticClass(), PlayerStarts);
	for (const auto& PlayerStart : PlayerStarts)
	{
		auto LPlayerStart = StaticCast<ANSPlayerStart*>(PlayerStart);
		if (LPlayerStart && LPlayerStart->CanSpawn(FName()))
		{
			ValidPlayerStarts.Add(LPlayerStart);
		}
	}
	if (!ValidPlayerStarts.IsValidIndex(0)) { return nullptr; }
	ResultPlayerStart = ValidPlayerStarts[ FMath::RandRange(int32(0), int32(ValidPlayerStarts.Num()-1)) ];
	
	//Spawn player pawn
	//GetWorld()->SpawnActor(DefaultPawnClass, ResultPlayerStart->GetTransform());
	auto NewPawn = SpawnDefaultPawnFor(Controller, ResultPlayerStart);
	
	//possess on pawn
	Controller->Possess(NewPawn);
	AddSpawnedPLayerInTeamList(NewPawn);
	return NewPawn;
}

void ANSGameMode::AddSpawnedPLayerInTeamList(APawn* Pawn)
{
	//GetTeam name
	//todo

	if (auto GameState = GetGameState<ANSGameState>())
	{
		GameState->AddPlayerPawn(Pawn);	
	}
}

APawn* ANSGameMode::SpawnSpectator(APlayerController* Controller)
{
	//spawn spectator pawn
	APawn* SpawnedSpectator = nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controller;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spectator pawns into a map
	auto CastController = StaticCast<AActor*>(Controller);
	SpawnedSpectator = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, CastController->GetActorLocation(), Controller->GetControlRotation(), SpawnParams);

	//possess
	if (Controller->GetPawn())
	{
		Controller->UnPossess();
	}
	Controller -> Possess(SpawnedSpectator);

	
	//set spectator pawn
	//Controller -> ChangeState("Spectating");
	
	return SpawnedSpectator;
}
