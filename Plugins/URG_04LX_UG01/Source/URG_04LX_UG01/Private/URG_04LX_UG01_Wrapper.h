// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <map>
#include <thread>
#include <mutex>
#include "GenericPlatform/GenericPlatformProcess.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/RunnableThread.h"
#include "Urg_driver.h"
#include "math_utilities.h"
#include "CoreMinimal.h"

/**
 *
 */

class URG_04LX_UG01_Wrapper
{

public:
	URG_04LX_UG01_Wrapper();
	~URG_04LX_UG01_Wrapper();

	qrk::Urg_driver* mURG;
	std::map<std::string, float> mDegreeAndDistance;
	std::mutex mMutexDegreeAndDistance;
	float mScanRangeMin;
	float mScanRangeMax;
	bool mIsOpen;
	FThreadSafeBool mFShutdown;





	void Clear();
	bool OpenURG(float aScanRangeMin, float aScanRangeMax, qrk::Lidar::connection_type_t aConnectionType, const char* aDevice_or_ip_name, long aBaudrate_or_port_number);
	void StopURG();
	void ShutDown();


	void UpdateDegreeAndDistance();
	void SetScanRange(float aScanRangeMin = -90, float aScanRangeMax = 90);
	void URGLiveLoop();
	std::map<std::string, float> GetDegreeAndDistance();
};

