


#if WITH_AUTOMATION_TESTS

#include "CharacterTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "NSTestUtils.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "TestProxyActor.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY_STATIC(LogCharacterTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterCanBeDamaged, "NetworkShooter.Character.CanBeDamaged", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterHealthRestore, "NetworkShooter.Character.HealthRestoredAfterTime", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


bool FCharacterCanBeDamaged::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);

	const auto Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	TestNotNull("Pawn exist", Pawn);
	
	const auto ASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
	TestNotNull("ASC valid", ASC);

	const auto Proxy = NSTestUtils::SpawnTestProxyActor(World);
	TestNotNull("Proxy actor is valid", Proxy);
	
	const auto BeginHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());

	
	const auto EffectSpec = ASC->MakeOutgoingSpec(Proxy->CharacterTestData.DamageEffect, 1.f, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	
	const auto NewHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());
	TestTrue("Damaged", NewHealth < BeginHealth);
	
	return true;
}

bool FCharacterHealthRestore::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);

	const auto Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	TestNotNull("Pawn exist", Pawn);
	
	const auto ASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
	TestNotNull("ASC valid", ASC);

	const auto Proxy = NSTestUtils::SpawnTestProxyActor(World);
	TestNotNull("Proxy actor is valid", Proxy);

	const auto BeginHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());

	
	const auto EffectSpec = ASC->MakeOutgoingSpec(Proxy->CharacterTestData.HalfHpDamage, 1.f, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	const float HalfHp = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());
	TestTrue("HalfHP damaged", FMath::IsNearlyEqual(BeginHealth, HalfHp * 2.f));

	
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(15.f));
	ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([=]()
	{
		const float NewHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());
		
		UE_LOG(LogCharacterTest, Display, TEXT("Regened %f, before: %f"), NewHealth,  HalfHp);
		
		TestTrue("HealthRestored", NewHealth > HalfHp);
		return true;
	}));
	
	return true;
}


#endif
