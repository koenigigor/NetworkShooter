// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class NetworkShoter : ModuleRules
{
	public NetworkShoter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"GameplayTags", 
			"GameplayTasks", 
			"DeveloperSettings", 
			"AIModule",
			"OnlineSubsystem", 
			"OnlineSubsystemUtils",
			"Json",
			"JsonUtilities",
			"EnhancedInput",
			"Niagara",
			"PhysicsCore",
			"SlateCore",
			"Slate",
			"TavaUIUtils",
			
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"NetCore",
			"GameplayMessageRuntime"
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
		
		PublicIncludePaths.AddRange(new string[]
		{
			"NetworkShoter",
			"NetworkShoter/Settings",
			"NetworkShoter/Misc",
			"NetworkShoter/Actors",
			"NetworkShoter/HUD",
			
		});
		
		// Uncomment if you are using online features
		//PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
