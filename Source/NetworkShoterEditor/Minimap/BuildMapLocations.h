// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorBuildUtils.h"
#include "UObject/Object.h"
#include "WorldPartition/WorldPartitionBuilder.h"
#include "BuildMapLocations.generated.h"

class UBakedMapLocations;


UCLASS()
class NETWORKSHOTEREDITOR_API UBuildMapLocations : public UWorldPartitionBuilder
{
	GENERATED_BODY()
public:
	static void BuildMapLocationsCMD(const TArray<FString>& Strings, UWorld* World);

	/** Get loaded objects, add it into data */
	static void AccumulateData(UWorld* World, UBakedMapLocations* Data);

	
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

private:
	TObjectPtr<UBlueprint> Blueprint = nullptr;
	TObjectPtr<UBakedMapLocations> LocationsData = nullptr;
};
