// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildMapLocations.h"

#include "BuildUtils.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Minimap/MapLocationSpline.h"
#include "Minimap/MapBounds.h"
#include "UObject/SavePackage.h"
#include "WorldPartition/IWorldPartitionEditorModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogBuildMapLocations, All, All);

using namespace BuildUtils;

void UBuildMapLocations::BuildMapLocationsCMD(const TArray<FString>& Strings, UWorld* World)
{
	if (!World) return;

	const bool bUseWorldPartition = Strings.ContainsByPredicate([](const FString& String)
	{
		return String.Equals("WP", ESearchCase::IgnoreCase) || String.Equals("WorldPartition", ESearchCase::IgnoreCase);
	});
	const int32 AssetNameIndex = Strings.IndexOfByPredicate([](const FString& String)
	{
		return String.Contains("AssetName=", ESearchCase::IgnoreCase);
	});
	const int32 PathIndex = Strings.IndexOfByPredicate([](const FString& String)
	{
		return String.Contains("Path=", ESearchCase::IgnoreCase);
	});

	const FString BaseAssetName = AssetNameIndex != INDEX_NONE ? Strings[AssetNameIndex].RightChop(10) : "LocationData";
	const FString BasePath = PathIndex != INDEX_NONE ? GetPath(Strings[PathIndex].RightChop(5)) : "/Game/BakedData/";
	const FString AssetName = BaseAssetName + "_" + World->GetMapName();

	if (bUseWorldPartition && World->IsPartitionedWorld())
	{
		UE_LOG(LogBuildMapLocations, Display, TEXT("Bake World partition world"))

		//todo how pass BasePath and AssetName in builder (make proxy text file in Save folder?)

		// Run world partition builder for iterate all cells
		//FEditorBuildUtils::EditorBuild(World, "BakeLayersData");
	}
	else
	{
		UE_LOG(LogBuildMapLocations, Display, TEXT("Bake not World partition world"))

		UBlueprint* Blueprint = LoadBlueprint<UBakedMapLocations>(BasePath, AssetName);
		UBakedMapLocations* MapLocations = Blueprint->GeneratedClass.Get()->GetDefaultObject<UBakedMapLocations>();
		MapLocations->MapLocations.Empty();

		AccumulateData(World, MapLocations);

		Blueprint->GetPackage()->MarkPackageDirty();
	}
}

void UBuildMapLocations::AccumulateData(UWorld* World, UBakedMapLocations* Data)
{
	static int32 LocationIndex = 0;

	for (TActorIterator<AMapLocationSpline> It(World, AMapLocationSpline::StaticClass()); It; ++It)
	{
		AMapLocationSpline* LocationSpline = *It;

		UE_LOG(LogTemp, Display, TEXT("LocationSpline = %s"), *LocationSpline->LocationID)
		
		//todo multiple splines for non convex regions
		
		// Validate unique object
		const auto bAlreadyAdded = Data->MapLocations.ContainsByPredicate([&](const UMapLocation* Location)
		{
			return Location->LocationID.Equals(LocationSpline->LocationID);
		});
		if (bAlreadyAdded) continue;

		// Create location
		auto MapLocation = Data->MapLocations.Add_GetRef(NewObject<UMapLocation>(Data, *FString("Location" + LocationIndex++),
			EObjectFlags::RF_Public | RF_Standalone));

		MapLocation->LocationID = LocationSpline->LocationID;

		
		// Create bound
		auto LocationBound = NewObject<ULocationBound>(MapLocation);
		MapLocation->Location = LocationBound;

		TArray<FVector2D> Points;
		for (int32 i = 0; i < LocationSpline->Spline->GetNumberOfSplinePoints(); ++i)
		{
			const auto SplinePoint = LocationSpline->Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			Points.Add(FVector2D(SplinePoint));
		}
		LocationBound->InitSpline(Points);
	}
}

EEditorBuildResult UBuildMapLocations::ExecuteBuild(UWorld* World, FName Name)
{
	if (World->IsPartitionedWorld())
	{
		IWorldPartitionEditorModule& WorldPartitionEditorModule = FModuleManager::LoadModuleChecked<IWorldPartitionEditorModule>("WorldPartitionEditor");
		WorldPartitionEditorModule.RunBuilder(StaticClass(), World);
	}

	return EEditorBuildResult::Success;
}

bool UBuildMapLocations::PreRun(UWorld* World, FPackageSourceControlHelper& PackageHelper)
{
	//tmp use default names
	const FString BaseAssetName = "LocationData";
	const FString BasePath = "/Game/BakedData/";
	const FString AssetName = BaseAssetName + "_" + World->GetMapName();
	
	// Load blueprint
	Blueprint = LoadBlueprint<UBakedMapLocations>(BasePath, AssetName);
	LocationsData = Blueprint->GeneratedClass.Get()->GetDefaultObject<UBakedMapLocations>();
	LocationsData->MapLocations.Empty();

	return true;
}

bool UBuildMapLocations::RunInternal(UWorld* World, const FCellInfo& InCellInfo, FPackageSourceControlHelper& PackageHelper)
{
	AccumulateData(World, LocationsData);
	
	return true;
}

bool UBuildMapLocations::PostRun(UWorld* World, FPackageSourceControlHelper& PackageHelper, const bool bInRunSuccess)
{
	// Save blueprint
	UPackage* Package = Blueprint->GetPackage();
	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	
	UPackage::SavePackage(Package, nullptr, *PackageFileName, FSavePackageArgs());

	Blueprint = nullptr;
	LocationsData = nullptr;

	return true;
}
