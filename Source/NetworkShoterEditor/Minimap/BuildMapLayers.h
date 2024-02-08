// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorBuildUtils.h"
#include "WorldPartition/WorldPartitionBuilder.h"
#include "BuildMapLayers.generated.h"

class UMapLayersData;

/** Bake all layers on map in structure
 *	Call from cmd, Keys:
 *		WP - use word partition builder (iterate all cells) - Works until you not open blueprint in editor (can't override opened file)
 *		AssetName - Name of result asset (default "LayersData")
 *		Path - Path for save asset (default /Game/BakedData/)	*/
UCLASS()
class NETWORKSHOTEREDITOR_API UBuildMapLayers : public UWorldPartitionBuilder
{
	GENERATED_BODY()
public:
	static void BuildMapLayersCMD(const TArray<FString>& Strings, UWorld* World);
	static EEditorBuildResult ExecuteBuild(UWorld* World, FName Name);
	
public:
	// UWorldPartitionBuilder interface begin
	virtual ELoadingMode GetLoadingMode() const override { return ELoadingMode::IterativeCells2D; } //ELoadingMode::EntireWorld
	virtual bool RequiresCommandletRendering() const override { return false; };

protected:
	virtual bool PreRun(UWorld* World, FPackageSourceControlHelper& PackageHelper) override;
	virtual bool RunInternal(UWorld* World, const FCellInfo& InCellInfo, FPackageSourceControlHelper& PackageHelper) override;
	virtual bool PostRun(UWorld* World, FPackageSourceControlHelper& PackageHelper, const bool bInRunSuccess) override;
	// UWorldPartitionBuilder interface end

	/** Load or create UMapLayersData blueprint */
	static UBlueprint* LoadBlueprint(const FString& BasePath, const FString& AssetName);
	
	/** Get all loaded layer colliders, add in layers data unique one */
	static void AccumulateLayersData(UWorld* World, UMapLayersData* LayersData);

private:
	TObjectPtr<UBlueprint> Blueprint = nullptr;
	TObjectPtr<UMapLayersData> LayersData = nullptr;
};
