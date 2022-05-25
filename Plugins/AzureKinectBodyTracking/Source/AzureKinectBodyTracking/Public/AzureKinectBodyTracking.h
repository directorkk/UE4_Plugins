// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"
#include "Misc/Paths.h"
#include "Misc/App.h"

#include "Util.h"

#include <fstream>
#include <string>

#include "Modules/ModuleManager.h"

class FAzureKinectBodyTrackingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
