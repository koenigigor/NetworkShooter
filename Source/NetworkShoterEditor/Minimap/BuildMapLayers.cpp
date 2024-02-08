// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildMapLayers.h"

#include "AssetCompilingManager.h"
#include "EngineUtils.h"
#include "KismetCompilerModule.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Minimap/MapLayerGroup.h"
#include "Minimap/MinimapLayerCollider.h"
#include "UObject/SavePackage.h"
#include "WorldPartition/IWorldPartitionEditorModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogBuildMapLayers, All, All);

namespace
{
/** Actor bounds in local space, ref from PCGHelpers::GetActorLocalBounds */
FBox GetActorLocalBounds(AActor* Actor)
{
	FBox Box(EForceInit::ForceInit);

	const bool bIncludeFromChildActors = true;

	const FTransform& ActorToWorld = Actor->GetTransform();
	const FTransform WorldToActor = ActorToWorld.Inverse();

	Actor->ForEachComponent<UPrimitiveComponent>(bIncludeFromChildActors, [&](const UPrimitiveComponent* InPrimComp)
	{
		if (InPrimComp->IsCollisionEnabled())
		{
			const FTransform ComponentToActor = InPrimComp->GetComponentTransform() * WorldToActor;
			Box += InPrimComp->CalcBounds(ComponentToActor).GetBox();
		}
	});

	return Box;
}

FString GetPath(FString String)
{
	return String.EndsWith("/") ? String : String + "/";
}
}

void UBuildMapLayers::BuildMapLayersCMD(const TArray<FString>& Strings, UWorld* World)
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
	
	const FString BaseAssetName = AssetNameIndex != INDEX_NONE ? Strings[AssetNameIndex].RightChop(10) : "LayersData";
	const FString BasePath = PathIndex != INDEX_NONE ? GetPath(Strings[PathIndex].RightChop(5)) : "/Game/BakedData/";
	const FString AssetName = BaseAssetName + "_" + World->GetMapName();

	
	if (bUseWorldPartition && World->IsPartitionedWorld())
	{
    	UE_LOG(LogBuildMapLayers, Display, TEXT("Bake World partition world"))

		//todo how pass BasePath and AssetName in builder (make proxy text file in Save folder?)
		
    	// Run world partition builder for iterate all cells
    	FEditorBuildUtils::EditorBuild(World, "BakeLayersData");
    }
	else
	{
		UE_LOG(LogBuildMapLayers, Display, TEXT("Bake not World partition world"))

		UBlueprint* Blueprint = LoadBlueprint(BasePath, AssetName);
        UMapLayersData* LayersData = Blueprint->GeneratedClass.Get()->GetDefaultObject<UMapLayersData>();
        LayersData->LayerStacks.Empty();

		AccumulateLayersData(World, LayersData);
		Blueprint->GetPackage()->MarkPackageDirty();
	}
}

EEditorBuildResult UBuildMapLayers::ExecuteBuild(UWorld* World, FName Name)
{
	if (World->IsPartitionedWorld())
	{
		IWorldPartitionEditorModule& WorldPartitionEditorModule = FModuleManager::LoadModuleChecked<IWorldPartitionEditorModule>("WorldPartitionEditor");
		WorldPartitionEditorModule.RunBuilder(StaticClass(), World);
	}

	return EEditorBuildResult::Success;
}

bool UBuildMapLayers::PreRun(UWorld* World, FPackageSourceControlHelper& PackageHelper)
{
	//tmp use default names
	const FString BaseAssetName = "LayersData";
	const FString BasePath = "/Game/BakedData/";
	const FString AssetName = BaseAssetName + "_" + World->GetMapName();
	
	// Load blueprint
	Blueprint = LoadBlueprint(BasePath, AssetName);
	LayersData = Blueprint->GeneratedClass.Get()->GetDefaultObject<UMapLayersData>();
	LayersData->LayerStacks.Empty();

	return true;
}

bool UBuildMapLayers::RunInternal(UWorld* World, const FCellInfo& InCellInfo, FPackageSourceControlHelper& PackageHelper)
{
	AccumulateLayersData(World, LayersData);

	return true;
}

bool UBuildMapLayers::PostRun(UWorld* World, FPackageSourceControlHelper& PackageHelper, const bool bInRunSuccess)
{
	// Save blueprint
	UPackage* Package = Blueprint->GetPackage();
	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	
	UPackage::SavePackage(Package, nullptr, *PackageFileName, FSavePackageArgs());

	Blueprint = nullptr;
	LayersData = nullptr;

	return true;
}

UBlueprint* UBuildMapLayers::LoadBlueprint(const FString& BasePath, const FString& AssetName)
{
	const FString PackagePath = BasePath + AssetName;
	const FString AssetPath = "Blueprint'" + PackagePath + "." + AssetName + "'";
	
	UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *AssetPath, nullptr, LOAD_NoWarn | LOAD_NoRedirects);
	if (!Blueprint)
	{
		UE_LOG(LogBuildMapLayers, Display, TEXT("Add new blueprint"))
		
		UPackage* Package = CreatePackage(*PackagePath);

		// Find BP class to create
		UClass* BpClass = nullptr;
		UClass* BpGenClass = nullptr;
		FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler")
			.GetBlueprintTypesForClass(UMapLayersData::StaticClass(), BpClass, BpGenClass);

		// Create asset
		Blueprint = FKismetEditorUtilities::CreateBlueprint(UMapLayersData::StaticClass(), Package, *AssetName, BPTYPE_Normal, BpClass, BpGenClass);

		FAssetRegistryModule::AssetCreated(Blueprint);
		Blueprint->GetPackage()->MarkPackageDirty();
	}

	return Blueprint;
}

void UBuildMapLayers::AccumulateLayersData(UWorld* World, UMapLayersData* LayersData)
{
	//index for generate unique Object package name
	static int32 StackIndex = 0;
	static int32 ConditionIndex = 0;

	for (TActorIterator<AActor> It(World, AActor::StaticClass()); It; ++It)
	{
		const auto Actor = *It;
		const auto Collider = Actor->FindComponentByClass<UMinimapLayerCollider>();
		if (!Collider) continue;

		auto StackName = Collider->LayerGroup;
		auto ColliderName = Collider->GetUniqueName();
		auto Floor = Collider->Floor;

		// Find or add stack (group of layers)
		const int32 Index = LayersData->LayerStacks.IndexOfByPredicate([&](const UMapLayerGroup* Stack)
		{
			return Stack->UniqueName.Equals(StackName);
		});

		UMapLayerGroup* Stack = Index != INDEX_NONE
			                        ? LayersData->LayerStacks[Index]
			                        : LayersData->LayerStacks.Add_GetRef(NewObject<UMapLayerGroup>(LayersData, *FString("Stack" + StackIndex++),
				                        EObjectFlags::RF_Public | RF_Standalone));
		if (Index == INDEX_NONE) Stack->UniqueName = StackName;

		auto& Sublayers = Stack->Floors.FindOrAdd(Floor).Sublayers;

		// Exit if collider (sublayer) already exist
		if (Sublayers.ContainsByPredicate([&](const FLayerSublayer& Sublayer)
		{
			return Sublayer.UniqueName == ColliderName;
		}))
		{
			continue;
		}

		//add sublayer
		FLayerSublayer NewSublayer;
		NewSublayer.UniqueName = ColliderName;

		//set visibility by copy constructor
		NewSublayer.VisibilityCondition = Collider->VisibilityCondition
			                                  ? NewObject<ULayerVisibilityCondition>(
				                                  LayersData,
				                                  Collider->VisibilityCondition->GetClass(),
				                                  *FString("Condition" + ConditionIndex++),
				                                  EObjectFlags::RF_Public | RF_Standalone,
				                                  Collider->VisibilityCondition)
			                                  : nullptr;

		//setup bounds
		if (Collider->OptionalStaticLayerBounds.IsEmpty())
		{
			FBox LocalBoundBox = GetActorLocalBounds(Actor);

			FLayerBounds Bounds;
			Bounds.Transform = Actor->GetActorTransform();
			Bounds.Center = LocalBoundBox.GetCenter();
			Bounds.LocalExtend = LocalBoundBox.Max;
			NewSublayer.Bounds.Add(Bounds);
		}
		else
		{
			for (auto BoundActor : Collider->OptionalStaticLayerBounds)
			{
				FBox LocalBoundBox = GetActorLocalBounds(BoundActor);

				FLayerBounds Bounds;
				Bounds.Transform = BoundActor->GetActorTransform();
				Bounds.Center = LocalBoundBox.GetCenter();
				Bounds.LocalExtend = LocalBoundBox.Max;
				NewSublayer.Bounds.Add(Bounds);

				if (!BoundActor->IsEditorOnly())
				{
					UE_LOG(LogBuildMapLayers, Display,
						TEXT("Optional bounds actor can be editor only, OptionalBound = %s, OwningCollider = %s, LayersGroup = %s"), *BoundActor->GetName(),
						*ColliderName, *StackName)
				}
			}
		}

		Sublayers.Add(NewSublayer);
	}
}
