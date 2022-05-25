// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "URG_04LX_UG01.h"

#define LOCTEXT_NAMESPACE "FURG_04LX_UG01Module"

void FURG_04LX_UG01Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FURG_04LX_UG01Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FURG_04LX_UG01Module, URG_04LX_UG01)