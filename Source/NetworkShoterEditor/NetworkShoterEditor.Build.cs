// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class NetworkShoterEditor : ModuleRules
{
	public NetworkShoterEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine",
			"NetworkShoter",
			"Json",
			"JsonUtilities",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			
		});
		
		PublicIncludePaths.AddRange(new string[]
		{
			"NetworkShoterEditor",

		});
	}
}
