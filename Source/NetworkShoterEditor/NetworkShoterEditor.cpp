// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkShoterEditor.h"

#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "UnrealEd.h"
#include "Minimap/BuildMapLayers.h"
#include "Minimap/BuildMapObjects.h"

IMPLEMENT_MODULE(FNetworkShoterEditorModule, NetworkShoterEditor);

void FNetworkShoterEditorModule::StartupModule()
{
	FEditorBuildUtils::RegisterCustomBuildType("BakeLayersData", FDoEditorBuildDelegate::CreateStatic(&UBuildMapLayers::ExecuteBuild), NAME_None);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Tava.Minimap.BakeLayers"),
		TEXT("Build Tava map layers data in data object, Keys: \n WP - use word partition builder (iterate all cells) - Works until you not open blueprint in editor (can't override opened file) \n AssetName - Name of result asset (default \"LayersData\") \n Path - Path for save asset (default /Game/BakedData/)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic( &UBuildMapLayers::BuildMapLayersCMD),
		ECVF_Cheat
	);

	FEditorBuildUtils::RegisterCustomBuildType("BakeMapObjectsData", FDoEditorBuildDelegate::CreateStatic(&UBuildMapObjects::ExecuteBuild), NAME_None);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Tava.Minimap.BakeMapObjects"),
		TEXT("Build Tava map map objects in data object, Keys: \n WP - use word partition builder (iterate all cells) - Works until you not open blueprint in editor (can't override opened file) \n AssetName - Name of result asset (default \"MapObjectsData\") \n Path - Path for save asset (default /Game/BakedData/)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic( &UBuildMapObjects::BuildMapLayersCMD),
		ECVF_Cheat
	);
}

void FNetworkShoterEditorModule::ShutdownModule()
{
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("Tava.Minimap.BakeLayers"));
	FEditorBuildUtils::UnregisterCustomBuildType("BakeLayersData");
	
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("Tava.Minimap.BakeMapObjects"));
	FEditorBuildUtils::UnregisterCustomBuildType("BakeMapObjectsData");
}
