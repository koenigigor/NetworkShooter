// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapBakeData.h"

#include "EditorBuildUtils.h"
#include "WorldPartition/IWorldPartitionEditorModule.h"



void UMinimapBakeData::BuildMinimapCMD(const TArray<FString>& Strings, UWorld* World)
{
	FEditorBuildUtils::EditorBuild(World, "MinimapBakeData");
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
	return true;
}

bool UMinimapBakeData::RunInternal(UWorld* World, const FCellInfo& InCellInfo, FPackageSourceControlHelper& PackageHelper)
{
	UE_LOG(LogTemp, Display, TEXT("LogWorldPartitionBuilder: UBakeMapData::RunInternal"))
	return true;
}

bool UMinimapBakeData::PostRun(UWorld* World, FPackageSourceControlHelper& PackageHelper, const bool bInRunSuccess)
{
	UE_LOG(LogTemp, Display, TEXT("LogWorldPartitionBuilder: UBakeMapData::PostRun"))
	return true;
}


