
#if WITH_AUTOMATION_TESTS

#include "MathTests.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"



IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestDevideAndRound, "NetworkShooter.Learn.DevideAndRound",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestArrayFilter, "NetworkShooter.Learn.ArrayFilder",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestArrayInsert, "NetworkShooter.Learn.ArrayInsert",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


bool FTestDevideAndRound::RunTest(const FString& Parameters)
{
	const int32 PlayersCount = 7;
	const int32 CountOfTeams = 2;
	
	int32 PlayersPerTeam = PlayersCount / CountOfTeams;
	if (PlayersCount % CountOfTeams != 0)
		PlayersPerTeam++;

	const int32 PlayersPerTeamMath = FMath::DivideAndRoundUp(PlayersCount, CountOfTeams);

	TestTrueExpr(PlayersPerTeam == PlayersPerTeamMath);
	return true;
}

bool FTestArrayFilter::RunTest(const FString& Parameters)
{
	TArray<int32> TestedArray = {1, 2, 4, 5, 6, 7};
	TArray<int32> FilteredArray = TestedArray.FilterByPredicate([](int32 Element)
	{
		return Element < 3;	
	});

	TestTrueExpr(FilteredArray.Num() == 2);
	return true;
}

bool FTestArrayInsert::RunTest(const FString& Parameters)
{
	TArray<int32> TestedArray = {1, 2, 4, 5, 6, 7};

	TestedArray.Insert(3, 2);
	for (const auto& Item : TestedArray)
	{
		UE_LOG(LogTemp, Display, TEXT("%d"), Item)
	}

	UE_LOG(LogTemp, Display, TEXT("------"))

	TestedArray.RemoveSingle(2);
	for (const auto& Item : TestedArray)
	{
		UE_LOG(LogTemp, Display, TEXT("%d"), Item)
	}

	/*
	UE_LOG(LogTemp, Display, TEXT("------"))

	TestedArray.Insert(3, 10);
	for (const auto& Item : TestedArray)
	{
		UE_LOG(LogTemp, Display, TEXT("%d"), Item)
	}
	*/


	return true;
}



#endif
