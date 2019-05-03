// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class DawitAndGoliath : ModuleRules
{
	public DawitAndGoliath(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "OnlineSubsystem",
            "OnlineSubsystemNull",
            "OnlineSubsystemUtils",
            "UMG",
            "Slate",
            "SlateCore"
        }
        );

		PrivateDependencyModuleNames.AddRange(new string[] { "Engine" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystem");
        PrivateDependencyModuleNames.Add("OnlineSubsystemNull");
        PrivateDependencyModuleNames.Add("OnlineSubsystemUtils");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
