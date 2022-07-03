// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ShooterPlayer.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GAS/NSAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "Game/NSPlayerState.h"
#include "Game/PCNetShooter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


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

	GiveStartupAbilities();
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

void AShooterPlayer::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (const auto PC = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping.InputMappingContext, 0);
		}
	}
}

//~==============================================================================================
// Ability System

UAbilitySystemComponent* AShooterPlayer::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AShooterPlayer::GiveStartupAbilities()
{
	if (!HasAuthority()) return;
	
	for (const auto& Ability : StartupAbilities)
	{
		GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability));
	}
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
	GetCharacterMovement() -> MaxWalkSpeed = Data.NewValue;

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

	if (HasAuthority())
	{
		AbilitySystem->ClearAllAbilities();
	}

	if (const auto PC = GetController<APCNetShooter>())
	{
		PC->SpawnSpectator();
		//Controller->ChangeState(NAME_Spectating);
	}
	
	BP_CharacterDead();
}

FGenericTeamId AShooterPlayer::GetGenericTeamId() const
{
	return FGenericTeamId::GetTeamIdentifier(GetPlayerState());
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

	if (Controller)
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

	if (const auto EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (InputMapping.InputActions.IA_Jump)
		{
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (InputMapping.InputActions.IA_Crouch)
		{
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Crouch, ETriggerEvent::Started, this, &ACharacter::Crouch, false);
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Crouch, ETriggerEvent::Completed, this, &ACharacter::UnCrouch, false);
		}
		if (InputMapping.InputActions.IA_Move)
		{
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Move, ETriggerEvent::Triggered, this, &ThisClass::MovementInput);
		}
		if (InputMapping.InputActions.IA_Rotation)
		{
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Rotation, ETriggerEvent::Triggered, this, &ThisClass::InputRotation);
		}

		//wanna find input action by name and bind in ASC input directly
		if (InputMapping.InputActions.IA_Confirm && AbilitySystem)
		{
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Confirm, ETriggerEvent::Started, AbilitySystem, &UNSAbilitySystemComponent::InputConfirm);
		}
		
		if (InputMapping.InputActions.IA_Cancel && AbilitySystem)
		{
			EnhancedInput->BindAction(InputMapping.InputActions.IA_Cancel, ETriggerEvent::Started, AbilitySystem, &UNSAbilitySystemComponent::InputCancel);
		}

		for (const auto& AbilityInput : InputMapping.AbilityInputs)
		{
			EnhancedInput->BindAction(AbilityInput, ETriggerEvent::Started, this, &ThisClass::InputTagPress, AbilityInput->InputTag);
			EnhancedInput->BindAction(AbilityInput, ETriggerEvent::Completed, this, &ThisClass::InputTagRelease, AbilityInput->InputTag);
		}
	}
}

void AShooterPlayer::MovementInput(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const FRotator Rotator = FRotator(0.f, GetControlRotation().Yaw, 0.f); 
	
	if (!FMath::IsNearlyZero(Value.X))
	{
		AddMovementInput(Rotator.Vector(), Value.X);
	}
	if (!FMath::IsNearlyZero(Value.Y))
	{
		AddMovementInput(UKismetMathLibrary::GetRightVector(Rotator), Value.Y);
	}
}

void AShooterPlayer::InputRotation(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void AShooterPlayer::InputTagPress(FGameplayTag Tag)
{
	if (AbilitySystem)
	{
		AbilitySystem->InputTagPressed(Tag);
	}
}

void AShooterPlayer::InputTagRelease(FGameplayTag Tag)
{
	if (AbilitySystem)
	{
		AbilitySystem->InputTagReleased(Tag);
	}
}

