// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ShooterPlayer.h"
#include "AbilitySystemComponent.h"
#include "NSAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "Game/NSPlayerState.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"


AShooterPlayer::AShooterPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UNSAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	CharacterAttributeSet = CreateDefaultSubobject<UNetShooterAttributeSet>(TEXT("CharacterAttributeSet"));
	WeaponAttributeSet = CreateDefaultSubobject<UWeaponAttributeSet>(TEXT("WeaponAttributeSet"));
}

void AShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	BindAttributeDelegates();

	//Give Startup abilities
	if (HasAuthority())
	{
		//cant bind key in network
		for (const auto& Ability : StartupAbilities)
		{
			GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability));
		}
	}
}

void AShooterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	}

	// ASC MixedMode replication requires that the ASC Owner's Owner be the Controller.
	SetOwner(NewController);
}

//~==============================================================================================
// Ability System

UAbilitySystemComponent* AShooterPlayer::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AShooterPlayer::AddAbility(TSubclassOf<UGameplayAbility> Ability)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("NoAuthority"));
	}
	
	GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability, 1, INDEX_NONE));
}

void AShooterPlayer::BindAttributeDelegates()
{
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CharacterAttributeSet->GetHealthAttribute()).AddUObject(this, &AShooterPlayer::OnHealthChange);
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CharacterAttributeSet->GetArmorAttribute()).AddUObject(this, &AShooterPlayer::OnHealthChange);
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CharacterAttributeSet->GetWalkSpeedAttribute()).AddUObject(this, &AShooterPlayer::OnWalkSpeedChange);
}

void AShooterPlayer::OnHealthChange(const FOnAttributeChangeData& Data)
{
	HealthChanged(Data.OldValue);
}

void AShooterPlayer::OnArmorChange(const FOnAttributeChangeData& Data)
{
	
}

void AShooterPlayer::OnWalkSpeedChange(const FOnAttributeChangeData& Data)
{
	WalkSpeedChanged(Data.OldValue);
}


/** [Multicast] Called from death gameplay ability, when player Die */
void AShooterPlayer::Death_Implementation()
{
	//notify player state on server and clients
	if (auto NSPlayerState = GetPlayerState<ANSPlayerState>())
	{
		NSPlayerState->OnCharacterDeath();
	}
	
	BP_CharacterDead();
}

ETeamAttitude::Type AShooterPlayer::Test_GetTeamAttitudeTowards(const AActor* Other)
{
	return GetTeamAttitudeTowards(*Other);
}

FGenericTeamId AShooterPlayer::GetGenericTeamId() const
{
	if ( auto TeamPlayerState = Cast<IGenericTeamAgentInterface>(GetPlayerState()))
	{
		return TeamPlayerState->GetGenericTeamId();
	}
	
	return IGenericTeamAgentInterface::GetGenericTeamId();
}

FVector AShooterPlayer::GetPawnViewLocation() const
{
	auto Camera = FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	
	return Super::GetPawnViewLocation();
}

FRotator AShooterPlayer::GetViewRotation() const
{
	//return Super::GetViewRotation();

	if (Controller != nullptr)
	{
		return Controller->GetControlRotation();
	}
	else if (GetLocalRole() < ROLE_Authority)
	{
		// check if being spectated
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController &&
				PlayerController->PlayerCameraManager &&
				PlayerController->PlayerCameraManager->GetViewTargetPawn() == this)
			{
				return PlayerController->BlendedTargetViewRotation;
			}
		}
	}

	
	//if pawn not local client, and player controlled return camera rotation who bind with control rotation
	if (auto Camera = FindComponentByClass<UCameraComponent>())
	{
		return Camera->GetComponentRotation();
	}

	
	return GetActorRotation();
}

void AShooterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

