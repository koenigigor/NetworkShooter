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

	GetGameState<ANSGameState>() -> RemovePawn(WhoKilled);
}

TArray<ANSPlayerStart*> ANSGameMode::GetFreePlayerStarts(FName CommandName)
{
	TArray<ANSPlayerStart*> FreePoints;

	//Get All Actors of class
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANSPlayerStart::StaticClass(), PlayerStarts);

	for (const auto& PlayerStart : PlayerStarts)
	{
		auto NSPlayerStart = StaticCast<ANSPlayerStart*>(PlayerStart);  //here StaticCast always valid
		if (NSPlayerStart -> CanSpawn(CommandName))
		{
			FreePoints.Add(NSPlayerStart);
		}
	}
	
	return FreePoints;
}

APawn* ANSGameMode::SpawnPlayer(APlayerController* Controller)
{
	//TODO Get TEAM
	FName Team = "Team1";
	
	TArray<ANSPlayerStart*> ValidPlayerStarts = GetFreePlayerStarts(Team);
	if (!ValidPlayerStarts.IsValidIndex(0)) { return nullptr; }

	//get random player start
	ANSPlayerStart* ResultPlayerStart = ValidPlayerStarts[FMath::RandRange(int32(0), int32(ValidPlayerStarts.Num()-1))];
	
	//Spawn player pawn
	//GetWorld()->SpawnActor(DefaultPawnClass, ResultPlayerStart->GetTransform());
	auto NewPawn = SpawnDefaultPawnFor(Controller, ResultPlayerStart);
	
	Controller->Possess(NewPawn);

	//add pawn in team list
	if (auto NSGameState = GetGameState<ANSGameState>())
	{
		NSGameState -> AddPlayerPawn(NewPawn);
	}
	
	return NewPawn;
}

/*
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
}*/
