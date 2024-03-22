// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KismetCompilerModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/Object.h"

DEFINE_LOG_CATEGORY_STATIC(LogBuildUtils, All, All);


namespace BuildUtils
{
	inline FString GetPath(FString String)
	{
		return String.EndsWith("/") ? String : String + "/";
	}

	/** Load or create UMapLayersData blueprint */
	template<class T>
	static UBlueprint* LoadBlueprint(const FString& BasePath, const FString& AssetName)
	{
		const FString PackagePath = BasePath + AssetName;
		const FString AssetPath = "Blueprint'" + PackagePath + "." + AssetName + "'";
	
		UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *AssetPath, nullptr, LOAD_NoWarn | LOAD_NoRedirects);
		if (!Blueprint)
		{
			UE_LOG(LogBuildUtils, Display, TEXT("Add new blueprint"))
		
			UPackage* Package = CreatePackage(*PackagePath);

			// Find BP class to create
			UClass* BpClass = nullptr;
			UClass* BpGenClass = nullptr;
			FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler")
				.GetBlueprintTypesForClass(T::StaticClass(), BpClass, BpGenClass);

			// Create asset
			Blueprint = FKismetEditorUtilities::CreateBlueprint(T::StaticClass(), Package, *AssetName, BPTYPE_Normal, BpClass, BpGenClass);

			FAssetRegistryModule::AssetCreated(Blueprint);
			Blueprint->GetPackage()->MarkPackageDirty();
		}

		return Blueprint;
	}
};
