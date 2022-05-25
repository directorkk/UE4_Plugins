// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class AzureKinectBodyTracking : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
	}


	public AzureKinectBodyTracking(ReadOnlyTargetRules Target) : base(Target)
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
				"Projects"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		LoadKinect(Target);
	}

	public bool LoadKinect(ReadOnlyTargetRules Target)
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
		string BinariesPath = "";

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			isLibrarySupported = true;

			string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
			string KinectPath = Path.Combine(ThirdPartyPath, "kinect-1.3.0");

			IncludePath = Path.Combine(KinectPath, "include");
			LibrariesPath = Path.Combine(KinectPath, "lib", PlatformString);
			BinariesPath = Path.Combine(KinectPath, "bin", PlatformString);


			Console.WriteLine("... IncludePath -> " + IncludePath);
			Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
		}

		if (isLibrarySupported)
		{
			// Include path
			PublicIncludePaths.Add(IncludePath);

			// Library path
			PublicLibraryPaths.Add(LibrariesPath);
			PublicLibraryPaths.Add(BinariesPath);

			// Add static libraries
			PublicAdditionalLibraries.Add("k4a.lib");
			PublicAdditionalLibraries.Add("k4abt.lib");
			PublicAdditionalLibraries.Add("k4arecord.lib");

			// Add dynamic libraries
			string[] dllList = {
				"k4a.dll",
				"k4abt.dll",
				"k4arecord.dll",
				"onnxruntime.dll",
				"depthengine_2_0.dll",
				"cudnn64_7.dll",
				"cublas64_100.dll",
				"cudart64_100.dll",
			};
			foreach(string dllName in dllList)
			{
				PublicDelayLoadDLLs.Add(dllName);
			}
		}

		Definitions.Add(string.Format("WITH_KINECT_BINDING={0}", isLibrarySupported ? 1 : 0));

		return isLibrarySupported;
	}
}
