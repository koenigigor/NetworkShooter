// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkShoterEditor.h"

#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "UnrealEd.h"
#include "Minimap/MinimapBakeData.h"

IMPLEMENT_MODULE(FNetworkShoterEditorModule, NetworkShoterEditor);

void FNetworkShoterEditorModule::StartupModule()
{
	// register bake minimap data
	FEditorBuildUtils::RegisterCustomBuildType("MinimapBakeData", FDoEditorBuildDelegate::CreateStatic(&UMinimapBakeData::ExecuteBuild), NAME_None);
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("Tava.Minimap.BakeData"),
		TEXT("Build Tava map data"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic( &UMinimapBakeData::BuildMinimapCMD),
		ECVF_Cheat
	);
}

void FNetworkShoterEditorModule::ShutdownModule()
{
	// unregister bake minimap data
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("Tava.Minimap.BakeData"));
	FEditorBuildUtils::UnregisterCustomBuildType("MinimapBakeData");
}
