// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorBuildUtils.h"
#include "WorldPartition/WorldPartitionBuilder.h"
#include "MinimapBakeData.generated.h"


/** Build minimap data (layers, icons, backgrounds) in file
 *	For world partition world */
UCLASS()
class NETWORKSHOTEREDITOR_API UMinimapBakeData : public UWorldPartitionBuilder
{
	GENERATED_BODY()
public:
	static void BuildMinimapCMD(const TArray<FString>& Strings, UWorld* World);
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
	

};
