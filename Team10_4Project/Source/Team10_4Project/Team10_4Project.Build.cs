// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Team10_4Project : ModuleRules
{
	public Team10_4Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",

            // GAS 모듈
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",

            // State Tree (UE 5.6 정식)
            "StateTreeModule",

            // 네트워킹
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "OnlineSubsystemEOS",
            "Sockets",
            "Networking",

            // UI
            "UMG",
            "Slate",
            "SlateCore",

            // UE 5.6 권장
            "CommonUI",
            "AIModule",
            "NavigationSystem",


            "GameplayStateTreeModule",

            // Voice Chat
            "VoiceChat",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        PublicIncludePaths.AddRange(new string[] {
			"Team10_4Project",
			"Team10_4Project/Variant_Platforming",
			"Team10_4Project/Variant_Platforming/Animation",
			"Team10_4Project/Variant_Combat",
			"Team10_4Project/Variant_Combat/AI",
			"Team10_4Project/Variant_Combat/Animation",
			"Team10_4Project/Variant_Combat/Gameplay",
			"Team10_4Project/Variant_Combat/Interfaces",
			"Team10_4Project/Variant_Combat/UI",
			"Team10_4Project/Variant_SideScrolling",
			"Team10_4Project/Variant_SideScrolling/AI",
			"Team10_4Project/Variant_SideScrolling/Gameplay",
			"Team10_4Project/Variant_SideScrolling/Interfaces",
			"Team10_4Project/Variant_SideScrolling/UI"
		});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDependencyModuleNames.Add("EOSVoiceChat");
        }

    }
}
