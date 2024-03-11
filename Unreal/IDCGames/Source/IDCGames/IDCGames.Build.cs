// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IDCGames : ModuleRules
{
	public IDCGames(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"HTTP",
				"Json",
				"OpenSSL",
				"JsonUtilities",
				"UMG"				
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Blutility",
				"UMGEditor",
				"UnrealEd",
				"DeveloperToolSettings",
				"DeveloperSettings",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		/// Added to support libcurl.
		AddEngineThirdPartyPrivateStaticDependencies(Target, "libcurl");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");

		if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 1)
		{
			AddEngineThirdPartyPrivateStaticDependencies(Target, "nghttp2");
		}
	}
}
