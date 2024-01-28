// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapBakeData.h"

#include "AssetToolsModule.h"
#include "CollisionChannels.h"
#include "EditorBuildUtils.h"
#include "EngineUtils.h"
#include "JsonObjectConverter.h"
#include "PackageHelperFunctions.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Minimap/MapObjectComponent.h"
#include "Minimap/MinimapLayerCollider.h"
#include "Misc/FileHelper.h"
#include "UObject/SavePackage.h"
#include "WorldPartition/IWorldPartitionEditorModule.h"


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
}


namespace 
{
	//DataTable info
	FString AssetName = "DT_BakedMaps";
	FString PackagePath = "/Game/NetworkShoter/UI/Widgets/Minimap/DT_BakedMaps";
	FString AssetPath = "/Script/Engine.DataTable'/Game/NetworkShoter/UI/Widgets/Minimap/DT_BakedMaps.DT_BakedMaps'";

	//Json info
	FString GetJsonFolder() { return FPaths::ProjectSavedDir() + "TestData/"; };
}

void UMinimapBakeData::BuildMinimapCMD(const TArray<FString>& Strings, UWorld* World)
{
	if (!World) return;
	
	// create and reload datatable not work on post run, (because is not main tread)
		
	// create datatable if no exist
	if (!LoadObject<UDataTable>(nullptr, *AssetPath, nullptr, LOAD_NoWarn | LOAD_NoRedirects))
    {
    	UPackage* Package = CreatePackage(*PackagePath);
       		
    	UDataTable* DataTable = NewObject<UDataTable>(Package, *AssetName, EObjectFlags::RF_Public | RF_Standalone);
    	DataTable->RowStruct = FBakedMapData::StaticStruct();
    
    	FAssetRegistryModule::AssetCreated(DataTable);
    	DataTable->GetPackage()->MarkPackageDirty();
    }

	// bake world partition world
	{
		FEditorBuildUtils::EditorBuild(World, "MinimapBakeData");
	}

	// reload datatable asset
	{
    	UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *AssetPath);
    	UPackage* Package = DataTable->GetPackage();
    	UPackageTools::ReloadPackages({Package});
	}
}

EEditorBuildResult UMinimapBakeData::ExecuteBuild(UWorld* InWorld, FName Name)
{
	if (InWorld->IsPartitionedWorld())
	{
		IWorldPartitionEditorModule& WorldPartitionEditorModule = FModuleManager::LoadModuleChecked<IWorldPartitionEditorModule>("WorldPartitionEditor");
		WorldPartitionEditorModule.RunBuilder(StaticClass(), InWorld);
	}
	
	return EEditorBuildResult::Success;
}


//


bool UMinimapBakeData::PreRun(UWorld* World, FPackageSourceControlHelper& PackageHelper)
{
	UE_LOG(LogTemp, Display, TEXT("LogWorldPartitionBuilder: UBakeMapData::PreRun"))

	Data.LevelName = World->GetMapName();
	
	return true;
}

bool UMinimapBakeData::RunInternal(UWorld* World, const FCellInfo& InCellInfo, FPackageSourceControlHelper& PackageHelper)
{
	UE_LOG(LogTemp, Display, TEXT("LogWorldPartitionBuilder: UBakeMapData::RunInternal"))

	// iterate actors with map components
	for (TActorIterator<AActor> It(World, AActor::StaticClass()); It; ++It)
	{
		const auto Actor = *It;
		const auto Collider = Actor->FindComponentByClass<UMinimapLayerCollider>();
		const auto MapObject = Actor->FindComponentByClass<UMapObjectComponent>();

		if (Collider && !BakedColliderIDs.Contains(Collider->GetUniqueID()))
		{
			BakedColliderIDs.Add(Collider->GetUniqueID());

			// add new collider data
			FVector Origin, Extent;
			Actor->GetActorBounds(false, Origin, Extent);
			const auto LocalBounds = GetActorLocalBounds(Actor);

			auto& LayerData = Data.Layers.AddDefaulted_GetRef();
			LayerData.Transform = Actor->GetActorTransform();
			LayerData.Center = LayerData.Transform.InverseTransformPosition(Origin);;
			LayerData.Extend = LocalBounds.Min.GetAbs() + LocalBounds.Max.GetAbs() * 0.5;
			LayerData.Layer = Collider->Layer;			
		}

		if (MapObject && !BakedObjectsIDs.Contains(Actor->GetUniqueID()))
		{
			BakedObjectsIDs.Add(Actor->GetUniqueID());
			
			// find collider for object
			FBakedMapLayerData* ColliderDataRef = GetLayerDataForPoint(Actor->GetActorLocation());
				
			// get data ref to data or data cache
			auto& ObjectData = ColliderDataRef ? ColliderDataRef->MapObjects.AddDefaulted_GetRef() : CachedObjectsData.AddDefaulted_GetRef();

			// fill map object data
			ObjectData.bIcon = MapObject->IsIcon();
			ObjectData.Image = MapObject->Image;
			ObjectData.Location = Actor->GetActorLocation();
			ObjectData.Size = MapObject->Size;
			ObjectData.FilterCategory = MapObject->FilterCategory;
			ObjectData.WidgetPriority = MapObject->WidgetPriority;
			//ObjectData.CustomWidgetClass; //todo
			//ObjectData.CustomWidgetBinaryData;
		}
	}
	
	return true;
}

bool UMinimapBakeData::PostRun(UWorld* World, FPackageSourceControlHelper& PackageHelper, const bool bInRunSuccess)
{
	UE_LOG(LogTemp, Display, TEXT("LogWorldPartitionBuilder: UBakeMapData::PostRun"))

	// add default layer (ground)
	auto& ZeroLayer = Data.Layers.AddDefaulted_GetRef();
	ZeroLayer.Layer = 0;

	// add cached map objects
	for (const auto& MapObjectData : CachedObjectsData)
	{
		//check collider again, if no collider add it as 0 layer
		if (const auto ColliderDataRef = GetLayerDataForPoint(MapObjectData.Location))
		{
			ColliderDataRef->MapObjects.Add(MapObjectData);
		}
		else
		{
			ZeroLayer.MapObjects.Add(MapObjectData);
		}
	} 
	
	//log result data
	{
		UE_LOG(LogTemp, Display, TEXT("Bake level: %s"), *Data.LevelName);
		for (const auto& Layer : Data.Layers)
		{
			UE_LOG(LogTemp, Display, TEXT("..Layer: %d"), Layer.Layer);
			UE_LOG(LogTemp, Display, TEXT("..Location: %s"), *Layer.Transform.GetLocation().ToString());
			UE_LOG(LogTemp, Display, TEXT("..Extend: %s"), *Layer.Extend.ToString());

			for (const auto& MapObject : Layer.MapObjects)
			{
				UE_LOG(LogTemp, Display, TEXT("....%s"), MapObject.bIcon ? *FString("Icon") : *FString("Background"));
				UE_LOG(LogTemp, Display, TEXT("....Priority: %d"), MapObject.WidgetPriority);
				UE_LOG(LogTemp, Display, TEXT("....Image: %s"), *MapObject.Image.ToString());

				UE_CLOG(MapObject.bIcon, LogTemp, Display, TEXT("....Category"), *MapObject.FilterCategory.ToString());
				//UE_CLOG(MapObject.CustomWidgetClass.IsValid(), LogTemp, Display, TEXT("....HasCutomWidget"));

				UE_LOG(LogTemp, Display, TEXT("..."));
			} 
		}
	}

	// export data
	ExportJson();
	WriteDatatable();
	
	return true;
}

FBakedMapLayerData* UMinimapBakeData::GetLayerDataForPoint(FVector IconPosition)
{
	//can be issues with not rectangle surfaces, (trace overlaps not works) (maybe need fill list in world on construction?)
	return Data.Layers.FindByPredicate([&](const FBakedMapLayerData& Layer)
	{
		const auto LocalCenter =  
		const auto IconLocalPosition = Layer.Transform.InverseTransformPosition(IconPosition) + Layer.Center;
		const auto Box = FBox(-Layer.Extend, Layer.Extend);
		return Box.IsInside(IconLocalPosition);
	});	
}

void UMinimapBakeData::ExportJson()
{
	const auto FileName = GetJsonFolder() + "MinimapData_" + Data.LevelName + ".Json";
		
	TSharedPtr<FJsonObject> MainJsonObject = FJsonObjectConverter::UStructToJsonObject(Data);
	if (!MainJsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Fail convert struct to object"))
		return;
	}

	FString OutputString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);

	if (!FJsonSerializer::Serialize(MainJsonObject.ToSharedRef(), JsonWriter))
	{
		UE_LOG(LogTemp, Error, TEXT("Fail serialalize json object to string"))
		return;
	}

	if (!JsonWriter->Close())
	{
		UE_LOG(LogTemp, Error, TEXT("Json writer closing error"))
		return;
	}

	if (!FFileHelper::SaveStringToFile(OutputString, *FileName))
	{
		UE_LOG(LogTemp, Error, TEXT("Fail save string in file %s"), *FileName);
		return;
	}
}

void UMinimapBakeData::WriteDatatable()
{
	UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *AssetPath);
	if (!DataTable) return;

	// add new map row in datatable
	DataTable->AddRow(FName(Data.LevelName), Data);

	// save datatable
	UPackage* Package = DataTable->GetPackage();
	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	UPackage::SavePackage(Package, nullptr, *PackageFileName, FSavePackageArgs());
}

