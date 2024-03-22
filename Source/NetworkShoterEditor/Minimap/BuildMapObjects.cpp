// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildMapObjects.h"

#include "BuildUtils.h"
#include "EngineUtils.h"
#include "KismetCompilerModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Minimap/MapObject.h"
#include "Minimap/MapObjectComponent.h"
#include "UObject/SavePackage.h"
#include "WorldPartition/IWorldPartitionEditorModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogBuildMapObjects, All, All);

using namespace BuildUtils;

void UBuildMapObjects::BuildMapLayersCMD(const TArray<FString>& Strings, UWorld* World)
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

	const FString BaseAssetName = AssetNameIndex != INDEX_NONE ? Strings[AssetNameIndex].RightChop(10) : "MapObjectsData";
	const FString BasePath = PathIndex != INDEX_NONE ? GetPath(Strings[PathIndex].RightChop(5)) : "/Game/BakedData/";
	const FString AssetName = BaseAssetName + "_" + World->GetMapName();

	if (false && bUseWorldPartition && World->IsPartitionedWorld())
	{
		UE_LOG(LogBuildMapObjects, Display, TEXT("Bake World partition world"))

		//todo how pass BasePath and AssetName in builder (make proxy text file in Save folder?)
		// TODO find level from baked layer bounds or else (trace not work)

		// Run world partition builder for iterate all cells
		FEditorBuildUtils::EditorBuild(World, "BakeMapObjectsData");
	}
	else
	{
		UE_LOG(LogBuildMapObjects, Display, TEXT("Bake not World partition world"))

		UBlueprint* Blueprint = LoadBlueprint<UBakedMapObjects>(BasePath, AssetName);
		UBakedMapObjects* BakedMapObjects = Blueprint->GeneratedClass.Get()->GetDefaultObject<UBakedMapObjects>();
		BakedMapObjects->MapObjects.Empty();

		AccumulateData(World, BakedMapObjects);
		Blueprint->GetPackage()->MarkPackageDirty();
	}
}

EEditorBuildResult UBuildMapObjects::ExecuteBuild(UWorld* World, FName Name)
{
	if (World->IsPartitionedWorld())
	{
		IWorldPartitionEditorModule& WorldPartitionEditorModule = FModuleManager::LoadModuleChecked<IWorldPartitionEditorModule>("WorldPartitionEditor");
		WorldPartitionEditorModule.RunBuilder(StaticClass(), World);
	}

	return EEditorBuildResult::Success;
}

bool UBuildMapObjects::PreRun(UWorld* World, FPackageSourceControlHelper& PackageHelper)
{
	//tmp use default names
	const FString BaseAssetName = "LayersData";
	const FString BasePath = "/Game/BakedData/";
	const FString AssetName = BaseAssetName + "_" + World->GetMapName();
	
	// Load blueprint
	Blueprint = LoadBlueprint<UBakedMapObjects>(BasePath, AssetName);
	BakedObjects = Blueprint->GeneratedClass.Get()->GetDefaultObject<UBakedMapObjects>();
	BakedObjects->MapObjects.Empty();

	return true;

}

bool UBuildMapObjects::RunInternal(UWorld* World, const FCellInfo& InCellInfo, FPackageSourceControlHelper& PackageHelper)
{
	AccumulateData(World, BakedObjects);
	
	return true;
}

bool UBuildMapObjects::PostRun(UWorld* World, FPackageSourceControlHelper& PackageHelper, const bool bInRunSuccess)
{
	// Save blueprint
	UPackage* Package = Blueprint->GetPackage();
	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	
	UPackage::SavePackage(Package, nullptr, *PackageFileName, FSavePackageArgs());

	Blueprint = nullptr;
	BakedObjects = nullptr;
	
	return true;
}

void UBuildMapObjects::AccumulateData(UWorld* World, UBakedMapObjects* Data)
{
	if (!World) return;

	//index for generate unique Object package name
	static int32 MapObjectIndex = 0;

	for (TActorIterator<AActor> It(World, AActor::StaticClass()); It; ++It)
	{
		const auto Actor = *It;
		const auto MapObjectComponent = Actor->FindComponentByClass<UMapObjectComponent>();
		if (!MapObjectComponent) continue;

		if (MapObjectComponent->MapObject && MapObjectComponent->bCanBake)
		{
			const auto MapObject = MapObjectComponent->MapObject;

			// Validate unique object
			const auto bAlreadyAdded = Data->MapObjects.ContainsByPredicate([&](const UMapObject* IteratedMapObject)
			{
				return IteratedMapObject->GetUniqueName().Equals(MapObject->GetUniqueName());
			});

			if (!bAlreadyAdded)
			{
				//copy constructor
				auto NewMapObject = NewObject<UMapObject>(Data,
					MapObject->GetClass(),
					*FString("Condition" + MapObjectIndex++),
					EObjectFlags::RF_Public | RF_Standalone,
					MapObject);

				Data->MapObjects.Add(NewMapObject);
				
				//initial params
				{
					NewMapObject->InitialLocation = Actor->GetActorLocation();

					// Not world partition world, can use trace
					MapObjectComponent->RefreshLayerVolumes();
					NewMapObject->LayerInfo = MapObjectComponent->MapObject->LayerInfo;
				}
			}
		}
	}
}
