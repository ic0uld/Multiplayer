// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActionShooter : ModuleRules
{
	public ActionShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
		
		PublicIncludePaths.Add("ActionShooter/");
		
		PrivateDependencyModuleNames.AddRange(new string[]{"GameplayAbilities", "GameplayTags", "GameplayTasks"});
	}
}
