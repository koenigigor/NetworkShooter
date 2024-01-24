// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TavaUIUtils : ModuleRules
{
	public TavaUIUtils(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] 
			{
				ModuleDirectory,
				
			});
				
		
		PrivateIncludePaths.AddRange(new string[] 
			{
				Path.Combine(ModuleDirectory, "EditableTextDecorator"),
				
			});
			
		
		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"UMG",
				
			});
			
		
		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore"
				
			});
		
		
		DynamicallyLoadedModuleNames.AddRange(new string[]
			{
				
			});
	}
}
