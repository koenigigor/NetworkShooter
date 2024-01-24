// Copyright Epic Games, Inc. All Rights Reserved.

#include "TavaUIUtilsEditor.h"

#include "FeedChildSnappingCustomization.h"

#define LOCTEXT_NAMESPACE "FTavaUIUtilsEditorModule"

void FTavaUIUtilsEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	//PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("EFeedChildSnapping"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFeedChildSnappingCustomization::MakeInstance));
}

void FTavaUIUtilsEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		//todo disabled because no good idea
		//PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("EFeedChildSnapping"));

		//PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTavaUIUtilsEditorModule, TavaUIUtilsEditor)