// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PCNetShooter.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "HUD/NSHUD.h"

//#include "WeaponAttributeSet.h"


/*
void APCNetShooter::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//get abilitycomponent
	auto ASC = InPawn->FindComponentByClass<UAbilitySystemComponent>();
	
	if (ASC)
	{
		UE_LOG(LogTemp,Warning, TEXT("ASC found"))
		//bind to ammo update
		auto Attributes = ASC->GetSpawnedAttributes();
		for (const auto& Attribute : Attributes)
		{
			if (auto WeaponAttribute = Cast<UWeaponAttributeSet>(Attribute))
			{
				ASC->GetGameplayAttributeValueChangeDelegate(WeaponAttribute->GetAmmoAttribute()).AddUObject(this, &APCNetShooter::OnAmmoUpdate);
				UE_LOG(LogTemp,Warning, TEXT("AmmoBound, %f"), WeaponAttribute->GetAmmo())
			}
		}

		//bind start reload
		//ASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &APCNetShooter::OnEffectAppliedToSelf);
		ASC->AbilityActivatedCallbacks.AddUObject(this, &APCNetShooter::OnAbilityActivate);
		UE_LOG(LogTemp,Warning, TEXT("ReloadBinding"))
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("ASC not found"))
	}
}


void APCNetShooter::OnAmmoUpdate(const FOnAttributeChangeData& Data)
{
	AmmoChange(Data.NewValue);
	UE_LOG(LogTemp,Warning, TEXT("Ammochanged"))
}

void APCNetShooter::OnAbilityActivate(UGameplayAbility* ActivatedAbility)
{
	UE_LOG(LogTemp,Warning, TEXT("Abbiltity tags %s"), *ActivatedAbility->AbilityTags.ToString())
	if (ActivatedAbility->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag("Skill.Reload")))
	{
		StartReload();
	}
}*/
void APCNetShooter::NotifyReceiveDamage_Implementation(float Damage, FVector FromDirection, FName InstigatorName,
	AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("PC: client receive damage %f, from %s"), Damage, *InstigatorName.ToString())
}

APawn* APCNetShooter::SpawnSpectator()
{
	//spawn spectator pawn
	APawn* SpawnedSpectator = nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spectator pawns into a map
	auto CastController = StaticCast<AActor*>(this);

	auto SpectatorClass = GetWorld()->GetGameState()->SpectatorClass;
	SpawnedSpectator = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, CastController->GetActorLocation(), GetControlRotation(), SpawnParams);

	bCanPossessWithoutAuthority = true;
	//possess
	if (GetPawn())
	{
		UnPossess();
	}
	Possess(SpawnedSpectator);

	bCanPossessWithoutAuthority = false;
	
	return SpawnedSpectator;
}

ANSHUD* APCNetShooter::GetNSHUD()
{
	if (!NSHUD) 
	{
		//protect for cast every call
		NSHUD = Cast<ANSHUD>(GetHUD());
	}
	
	return NSHUD;
}

void APCNetShooter::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//if it local client possess (spectator)
	if (!GetWorld()->IsServer())
	{
		OnRep_Pawn();
	}
}

void APCNetShooter::OnRep_Pawn()
{
	Super::OnRep_Pawn();
//пока не стоит объявлять делегат ради 2х функций
	
	BP_ClientOnPossess(GetPawn());
    
	GetNSHUD()->OnPossess(GetPawn());
}
