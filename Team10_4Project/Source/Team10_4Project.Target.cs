// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Team10_4ProjectTarget : TargetRules
{
	public Team10_4ProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("Team10_4Project");

        // 서버 최적화
        bUseLoggingInShipping = true;
        bCompileWithPluginSupport = true;
        bUseUnityBuild = true;
        bUsePCHFiles = true;
    }
}
