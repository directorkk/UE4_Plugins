// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class URG_04LX_UG01 : ModuleRules
{
	private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
	
	public URG_04LX_UG01(ReadOnlyTargetRules Target) : base(Target)
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
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
        LoadURG(Target);
    }

	public bool LoadURG(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        bool isDebug = false;
        if ((Target.Configuration == UnrealTargetConfiguration.DebugGame) ||
            (Target.Configuration == UnrealTargetConfiguration.Debug) ||
            (Target.Configuration == UnrealTargetConfiguration.Development))
        {
            isDebug = true;
        }
        string IncludePath = "";
        string LibrariesPath = "";
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string URGPath = Path.Combine(ThirdPartyPath, "URG1.2.5");

            IncludePath = Path.Combine(URGPath, "include");
            LibrariesPath = Path.Combine(URGPath, "lib", PlatformString);

            Console.WriteLine("... IncludePath -> " + IncludePath);
            Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(IncludePath);

            // Library path
            PublicLibraryPaths.Add(LibrariesPath);

            // Add static libraries
            PublicAdditionalLibraries.Add("urg_cpp.lib");
            PublicAdditionalLibraries.Add("urg.lib");
        }

        Definitions.Add(string.Format("WITH_URG_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}

