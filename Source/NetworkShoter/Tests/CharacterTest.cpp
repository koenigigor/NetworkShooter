#if WITH_AUTOMATION_TESTS

#include "CharacterTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Utils/NSTestUtils.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "Utils/TestProxyActor.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY_STATIC(LogCharacterTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterCanBeDamaged, "NetworkShooter.Character.CanBeDamaged",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterHealthRestore, "NetworkShooter.Character.HealthRestoredAfterTime",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

namespace
{
FString LevelName = "/Game/NetworkShoter/Tests/EmptyTestLevel";
}

bool FCharacterCanBeDamaged::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel(LevelName);

	UWorld* World = NSTestUtils::GetTestWorld();
	if (!TestNotNull("World exist", World)) return false;

	const auto Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!TestNotNull("Pawn exist", Pawn)) return false;

	const auto ASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
	if (!TestNotNull("ASC valid", ASC)) return false;

	const auto Proxy = NSTestUtils::SpawnTestProxyActor(World);
	if (!TestNotNull("Proxy actor is valid", Proxy)) return false;

	const auto DamageEffect = Proxy->CharacterTestData.DamageEffect;
	if (!TestTrue("DamageEffect is valid", DamageEffect != nullptr)) return false;
	//

	const auto BeginHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());

	const auto EffectSpec = ASC->MakeOutgoingSpec(DamageEffect, 1.f, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	const auto NewHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());
	TestTrue("Damaged", NewHealth < BeginHealth);

	return true;
}

bool FCharacterHealthRestore::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel(LevelName);

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);

	const auto Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	TestNotNull("Pawn exist", Pawn);

	const auto ASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
	TestNotNull("ASC valid", ASC);

	const auto Proxy = NSTestUtils::SpawnTestProxyActor(World);
	TestNotNull("Proxy actor is valid", Proxy);

	const auto HalfHpDamageEffect = Proxy->CharacterTestData.HalfHpDamage;
	if (!TestTrue("HalfHpDamageEffect is valid", HalfHpDamageEffect != nullptr)) return false;
	//	
	
	const auto BeginHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());

	const auto EffectSpec = ASC->MakeOutgoingSpec(HalfHpDamageEffect, 1.f, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	const float HalfHp = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());
	TestTrue("HalfHP damaged", FMath::IsNearlyEqual(BeginHealth, HalfHp * 2.f));

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(15.f));
	ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([=]()
		{
		const float NewHealth = ASC->GetNumericAttribute(UNetShooterAttributeSet::GetHealthAttribute());

		UE_LOG(LogCharacterTest, Display, TEXT("Regened %f, before: %f"), NewHealth, HalfHp);

		TestTrue("HealthRestored", NewHealth > HalfHp);
		return true;
		}));

	return true;
}


#endif
