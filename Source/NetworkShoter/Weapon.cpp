// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"


AWeapon::AWeapon()
{
	//PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//create component
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon");
	WeaponMesh->SetIsReplicated(true);
	RootComponent = WeaponMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetAutoActivate(false);
}

void AWeapon::SetupData(UWeaponData* Data)
{
	if (!Data)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon data not valid in weapon %s"), *GetName())
		return;
	}
	
	WeaponData = Data;
	
	//set mesh
	WeaponMesh->SetStaticMesh( Data->Mesh);
}

void AWeapon::LaunchAsProjectile(FVector Velocity)
{
	//ProjectileMovement->SetVelocityInLocalSpace(Velocity);
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->Activate();
}


/*
void AWeapon::DoPunch()
{
	if (!HasAuthority()) return;
	if (!CanPunch()) return;

	PerformPunch();
	StartPunchDelay();
	Punched.Broadcast();
}

void AWeapon::PerformPunch()
{
	if (!HasAuthority()) return;
	
	FVector StartTrace = GetActorLocation();
	FVector EndTrace;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> Hits;
	
	if (GetInstigatorController())
		{
			EndTrace = GetInstigatorController()->GetControlRotation().Vector() * PunchDestination + StartTrace;
		}
	else
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon has no Instigator"))
		}
		
		
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		StartTrace, EndTrace, PunchRadius,
		//ETraceTypeQuery::TraceTypeQuery1, //TODO ?
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore, EDrawDebugTrace::ForDuration,
		Hits, true);

	if (bHit)
	{
		for (const auto& Hit : Hits)
		{
			//think no need check dublicate
			DealDamage(Hit, PunchDamage);
		} 
	}
}

void AWeapon::DealDamage(FHitResult Hit, float Damage)
{
	if (!HasAuthority()) return;

	UGameplayStatics::ApplyPointDamage(
		Hit.GetActor(),
		Damage,
		Hit.ImpactNormal, Hit,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()); //default damage type?
}

void AWeapon::StartPunchDelay()
{
	bPunchOnDelay = true;
	FTimerHandle PunchDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(
		PunchDelayHandle,this, &AWeapon::EndPunchDelay,
		DelayBetweenPunches, false);
}

void AWeapon::EndPunchDelay()
{
	bPunchOnDelay = false;
}

bool AWeapon::CanPunch() const
{
	return (bCanPunch && !bPunchOnDelay);
}
*/
