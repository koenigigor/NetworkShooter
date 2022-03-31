// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ShooterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "Game/NSPlayerState.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"

// Sets default values
AShooterPlayer::AShooterPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	CharacterAttributeSet = CreateDefaultSubobject<UNetShooterAttributeSet>(TEXT("CharacterAttributeSet"));
	WeaponAttributeSet = CreateDefaultSubobject<UWeaponAttributeSet>(TEXT("WeaponAttributeSet"));
}

UAbilitySystemComponent* AShooterPlayer::GetAbilitySystemComponent() const
{
	return AbilitySystem;
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

FVector AShooterPlayer::GetPawnViewLocation() const
{
	auto Camera = FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		return Camera->GetComponentLocation();
	}
	
	return Super::GetPawnViewLocation();
}

// Called when the game starts or when spawned
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
}

void AShooterPlayer::OnHealthChange(const FOnAttributeChangeData& Data)
{
	HealthChanged(Data.OldValue);
}


void AShooterPlayer::OnArmorChange(const FOnAttributeChangeData& Data)
{
	//TODO
}

// Called every frame
void AShooterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShooterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

