// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AzureKinectBodyTracking.h"
using namespace AzureKinectBodyTracking;

#define LOCTEXT_NAMESPACE "FAzureKinectBodyTrackingModule"

void FAzureKinectBodyTrackingModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	/*FString dllPath = FPaths::Combine(FPaths::RootDir(), FApp::GetProjectName(), L"Content/DLLs");
	FString dllList[] = {
				"k4a.dll",
				"k4abt.dll",
				"k4arecord.dll",
				"onnxruntime.dll",
				"depthengine_2_0.dll",
				"cudnn64_7.dll",
				"cublas64_100.dll",
				"cudart64_100.dll",
	};
	std::ofstream output;
	output.open("D:/test.txt");
	for (int i = 0; i < 8; i++) {
		FString dllFullName = FPaths::Combine(dllPath, dllList[i]);
		void *handle = FPlatformProcess::GetDllHandle(*dllFullName);
		if (handle != nullptr) {
			std::string testt = Util::wstos(*dllFullName);
			output.write(testt.c_str(), testt.length());
		}
	}

	output.close();*/
}

void FAzureKinectBodyTrackingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAzureKinectBodyTrackingModule, AzureKinectBodyTracking)