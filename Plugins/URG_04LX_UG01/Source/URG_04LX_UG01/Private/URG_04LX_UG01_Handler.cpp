// Fill out your copyright notice in the Description page of Project Settings.


#include "URG_04LX_UG01_Handler.h"
#include "Engine/Engine.h"

// Sets default values
AURG_04LX_UG01_Handler::AURG_04LX_UG01_Handler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mURGWrapper = nullptr;
}

// Called when the game starts or when spawned
void AURG_04LX_UG01_Handler::BeginPlay()
{
	Clear();
	mURGResetWorker = nullptr;
	Super::BeginPlay();
}

void AURG_04LX_UG01_Handler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Shutdown();
}

// Called every frame
void AURG_04LX_UG01_Handler::Tick(float DeltaTime)
{
	if (!mIsInitialize)
	{
		return;
	}
	GetDegreeAndDistance(mOriginDegreeAndDistance);
	ModifyDegreeAndDistanceMap(mOriginDegreeAndDistance, mModifiedDegreeAndDistance);
	Super::Tick(DeltaTime);
}


void AURG_04LX_UG01_Handler::OpenURG(float aScanRangeMin, float aScanRangeMax, float aDetectDistanceMin, float aDetectDistanceMax, ConnectionType aConnectionType, FString aDevice_or_ip_name, int aBaudrate_or_port_number, bool& oIsOpen)
{
	const char* device_or_ip_name = TCHAR_TO_ANSI(*aDevice_or_ip_name);
	qrk::Lidar::connection_type_t connectionType;
	switch (aConnectionType)
	{
	case ConnectionType_Serial:
		connectionType = qrk::Lidar::connection_type_t::Serial;
		break;
	case ConnectionType_Ethernet:
		connectionType = qrk::Lidar::connection_type_t::Ethernet;
		break;
	}
	mDetectDistanceMin = aDetectDistanceMin;
	mDetectDistanceMax = aDetectDistanceMax;
	if (mURGWrapper->OpenURG(aScanRangeMin, aScanRangeMax, connectionType, device_or_ip_name, aBaudrate_or_port_number) == false)
	{
		oIsOpen = false;
	}
	else
	{
		StartURGLoopWorker();
		StartURGFilter();
		oIsOpen = true;
		for (float i = aScanRangeMin; i <= aScanRangeMax; i += 0.5f)
		{
			FString degree = FString::SanitizeFloat(i);
			mDegreeArray.Add(degree);
			mEnvironmentInfo.Add(degree, mDetectDistanceMax);
		}
	}
}

void AURG_04LX_UG01_Handler::OpenURGWithoutEnvironmentSetting(float aScanRangeMin, float aScanRangeMax, ConnectionType aConnectionType, FString aDevice_or_ip_name, int aBaudrate_or_port_number, bool & oIsOpen)
{
	const char* device_or_ip_name = TCHAR_TO_ANSI(*aDevice_or_ip_name);
	qrk::Lidar::connection_type_t connectionType;
	switch (aConnectionType)
	{
	case ConnectionType_Serial:
		connectionType = qrk::Lidar::connection_type_t::Serial;
		break;
	case ConnectionType_Ethernet:
		connectionType = qrk::Lidar::connection_type_t::Ethernet;
		break;
	}

	mURGResetWorker = new URGResetWorker(this, aScanRangeMin, aScanRangeMax, connectionType, device_or_ip_name, aBaudrate_or_port_number);
	mURGResetWorker->Init("URGResetWorker");
}

void AURG_04LX_UG01_Handler::StopURG()
{
	mURGWrapper->StopURG();
}

void AURG_04LX_UG01_Handler::GetDegreeAndDistance(TMap<FString, float>& oDegreeAndDistance)
{
	if (mIsURGReady) {
		return;
	}

	std::map<std::string, float> degreeAndDistance = mURGWrapper->GetDegreeAndDistance();
	for (std::map<std::string, float>::iterator it = degreeAndDistance.begin(); it != degreeAndDistance.end(); ++it)
	{
		FString degree = FString::SanitizeFloat(FCString::Atof(*FString(it->first.c_str()))*-1);
		float distance = it->second;
		oDegreeAndDistance.Add(degree, distance);
	}
}

void AURG_04LX_UG01_Handler::SetScanRange(int aScanRangeMin, int aScanRangeMax)
{
	if (mIsURGReady) {
		return;
	}
	mURGWrapper->SetScanRange(aScanRangeMin, aScanRangeMax);
}

void AURG_04LX_UG01_Handler::ModifyDegreeAndDistanceMap(TMap<FString, float> aOriginMap, TMap<FString, float>& oResultMap)
{
	TMap<FString, float> result;

	TArray<FString> keys;
	aOriginMap.GetKeys(keys);
	for (int i = 0; i < keys.Num(); i++)
	{
		FString key = keys[i];
		float   value = aOriginMap[key] / 10; //convert mm to cm
		if (value > mDetectDistanceMax)
		{
			mDetectDistanceMax = value;
		}
		else if (value <= 0) //URG return 0 mm when over max distnace range
		{
			value = mDetectDistanceMax;
		}
		else if (value < mDetectDistanceMin && value > 0)
		{
			value = -1; // URG return 0 ~ MinDistance when receive distance fail
		}
		
		result.Add(key, value);
	}
	std::lock_guard<std::mutex> lock(mMutexModifiedDegreeAndDistance);
	oResultMap = result;
}

void AURG_04LX_UG01_Handler::GetInteractionResult(TMap<FString, float> aDegreeAndDistance, TArray<FTarget> aInputTarget, TArray<FTarget>& oResultTarget)
{
	/*if (aDegreeAndDistance.Num() == 0)
	{
		return;
	}
	for (int i = 0; i < aInputTarget.Num(); i++)
	{
		oResultTarget.Add(aInputTarget[i]);
		FRotator actorRotation = GetActorRotation();
		FVector inputLocation = aInputTarget[i].TargetActor->GetActorLocation();
		//FVector inputLocation = aInputTarget[i].Position;
		FVector URGActorLocation = GetActorLocation();
		float   distance = FVector::Distance(inputLocation, URGActorLocation);
		float   angle = (acos(Dot3((inputLocation / distance), GetActorForwardVector())))*mDegPerRad;
		float	toleranceDistance = aInputTarget[i].InteractionToleranceDistance;
		float	toleranceAngle = aInputTarget[i].InteractionToleranceAngle;
		if (actorRotation.UnrotateVector(inputLocation / distance).Y < 0)//if location is left side
		{
			angle *= -1;
		}
		angle = round(angle - actorRotation.Yaw);
		for (float j = angle - toleranceAngle; j < angle + toleranceAngle; j += 0.5f)
		{
			if (abs(aDegreeAndDistance.FindRef(FString::SanitizeFloat(j)) - distance) <= toleranceDistance)
			{
				oResultTarget[i].InRange = true;
				break;
			}
			else
			{
				oResultTarget[i].InRange = false;
				break;
			}
		}
	}*/
}

void AURG_04LX_UG01_Handler::GetObstacle(TMap<FString, float> aDegreeAndDistance, TArray<FObstacle>& oObstacle)
{
	if (aDegreeAndDistance.Num() == 0)
	{
		return;
	}
	int averageCount = 1;
	int averageInsideCount = 1;
	int averageOutsideCount = 1;
	bool insideObstacle = false;
	bool isEnvironment = true;
	float average = 0;
	float lastAverage = 0;
	float averageInside = 0;
	float averageOutside = 0;
	FString InsideDegreeMin = "";
	FString InsideDegreeMax = "";
	TArray<FString> insideDegrees;
	TMap<FString, float> environmentInfo = GetEnvironmentInfo();
	for (int i = 0; i < mDegreeArray.Num(); i++)
	{
		FObstacle Obstacle;
		FString key = mDegreeArray[i];
		FVector position;
		float value = aDegreeAndDistance[key];
		if (value == -1)
		{
			if (insideObstacle && i == mDegreeArray.Num() - 1)
			{
				InsideDegreeMax = mDegreeArray[i - 1];
				FRotator rotator = FRotator(0, ((FCString::Atof(*InsideDegreeMax) + FCString::Atof(*InsideDegreeMin)) / 2), 0);
				FRotator actorRotation = GetActorRotation();
				FVector actorVector = GetActorLocation();
				position = rotator.RotateVector(FVector(lastAverage, 0, 0));
				position = actorRotation.RotateVector(position) + actorVector;
				Obstacle.DegreeMin = FCString::Atof(*InsideDegreeMin);
				Obstacle.DegreeMax = FCString::Atof(*InsideDegreeMax);
				Obstacle.AverageDistance = lastAverage;
				Obstacle.Position = position;
				oObstacle.Add(Obstacle);
			}
			continue;
		}
		float environmentValue = environmentInfo[key];
		if (value >= environmentValue)
		{
			value = environmentValue;
			isEnvironment = true;
		}
		else
		{
			isEnvironment = false;
		}
		if (i == 0)
		{
			average = environmentValue; //first value
		}
		lastAverage = average;
		float dif = abs(lastAverage - value);
		if (insideObstacle)
		{
			if (dif >= mObstacleThreshold || i == mDegreeArray.Num() - 1 || isEnvironment)
			{
				if (averageCount <= 0)
				{
					average = value;
					averageOutsideCount = averageCount;
					insideObstacle = false;
					for (int j = 0; j < insideDegrees.Num(); j++)
					{
						AddInsideObstacle(insideDegrees[j], insideObstacle);
					}
				}
				else
				{
					average = value;
					averageCount = 1;
					averageOutsideCount = averageCount;
					insideObstacle = false;
					InsideDegreeMax = mDegreeArray[i - 1];
					FRotator rotator = FRotator(0, ((FCString::Atof(*InsideDegreeMax) + FCString::Atof(*InsideDegreeMin)) / 2), 0);
					FRotator actorRotation = GetActorRotation();
					FVector actorVector = GetActorLocation();
					position = rotator.RotateVector(FVector(lastAverage, 0, 0));
					position = actorRotation.RotateVector(position) + actorVector;
					Obstacle.DegreeMin = FCString::Atof(*InsideDegreeMin);
					Obstacle.DegreeMax = FCString::Atof(*InsideDegreeMax);
					Obstacle.AverageDistance = lastAverage;
					Obstacle.Position = position;
					oObstacle.Add(Obstacle);
					InsideDegreeMin = "";
					InsideDegreeMax = "";
				}
				insideDegrees.Empty();
			}
			else
			{
				average = (average * averageCount + value) / (averageCount + 1);
				averageCount += 1;
				averageInside = average;
				averageInsideCount = averageCount;
				insideDegrees.Add(key);
			}
		}
		if(!insideObstacle)
		{
			if  ((dif >= mObstacleThreshold) && !isEnvironment)
			{
				average = value;
				averageCount = 1;
				averageInsideCount = averageCount;
				InsideDegreeMin = key;
				insideDegrees.Add(key);
				insideObstacle = true;
			}
			else
			{
				average = (average * averageCount + value) / (averageCount + 1);
				averageCount += 1;
				averageOutside = average;
				averageOutsideCount = averageCount;
			}
		}
		AddInsideObstacle(key, insideObstacle);
	}
}

void AURG_04LX_UG01_Handler::GetFilteredObstacle(TArray<FObstacle>& oObstacle)
{
	std::lock_guard<std::mutex> lock(mMutexObstacle);
	oObstacle = mObstacle;
}

void AURG_04LX_UG01_Handler::SaveEnvironmentInfo(TMap<FString, float> aEnvironmentInfo)
{
	if (aEnvironmentInfo.Num() > 0)
	{
		std::lock_guard<std::mutex> lock(mMutexEnvironmentInfo);
		for (int i = 0; i < mDegreeArray.Num(); i++)
		{
			FString key = mDegreeArray[i];
			mEnvironmentInfo.Add(key, aEnvironmentInfo[key] * 0.95f);
		}
	}
}

void AURG_04LX_UG01_Handler::GetInsideObstacle(TMap<FString, bool>& oInsideObstacle)
{
	std::lock_guard<std::mutex> lock(mMutexInsideObstacle);
	oInsideObstacle = mInsideObstacle;
}

void AURG_04LX_UG01_Handler::GetObstacleDown(TArray<FObstacle>& oObstacleDown)
{
	std::lock_guard<std::mutex> lock(mMutexObstacleDown);
	oObstacleDown = mObstacleDown;
	mObstacleDown.Empty();
}

void AURG_04LX_UG01_Handler::GetObstacleUp(TArray<FObstacle>& oObstacleUp)
{
	std::lock_guard<std::mutex> lock(mMutexObstacleUp);
	oObstacleUp = mObstacleUp;
	mObstacleUp.Empty();
}

void AURG_04LX_UG01_Handler::StartURGLoopWorker()
{
	if (mIsURGReady) {
		return;
	}
	mURGLoopWorker = new URGLoopWorker(mURGWrapper);
	mURGLoopWorker->Init("URGLoopWorker");
}

void AURG_04LX_UG01_Handler::StartURGResetWorker()
{
}

void AURG_04LX_UG01_Handler::StartURGFilter()
{
	mURGFilter = new URGFilter(this);
	mURGFilter->Init("URGFilter");
}

void AURG_04LX_UG01_Handler::URGFilterLiveLoop()
{
	TArray<FObstacle> obstacleDown;
	TArray<FObstacle> obstacleUp;
	TArray<FObstacle> obstacleLast;
	int maxId = -1;

	while (!mFShutdownFliter)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(mSleepInterval));
		if (!mIsInitialize)
		{
			continue;
		}
		TMap<FString, float> degreeAndDistance;
		TMap<FString, float> modifiedDegreeAndDistance = GetModifiedDegreeAndDistance();
		if (modifiedDegreeAndDistance.Num() == 0)
		{
			continue;
		}
		else
		{
			degreeAndDistance = modifiedDegreeAndDistance;
		}
		TArray<FObstacle> obstacle;
		GetObstacle(degreeAndDistance, obstacle);

		/*if (obstacleLast.Num() == 0) //如果全空 從0開始給ID
		{
			maxId = -1;
			for (int i = 0; i < obstacle.Num(); i++)
			{
				obstacle[i].Id = ++maxId;
				AddObstacleDown(obstacle[i]);
			}
			obstacleLast = obstacle;
			mObstacle = obstacle;
			continue;
		}*/

		TArray<FObstacle> obstacleNow = obstacle;


		int compareTimes = obstacleLast.Num();
		for (int i = 0; i < compareTimes; i++)
		{
			float minDistance = 100000;
			int matchIndexLast = -1;
			int matchIndexNow = -1;
			for (int j = 0; j < obstacleLast.Num(); j++)
			{
				if(obstacleLast[j].Id == -2)
				{
					continue;
				}
				for (int k = 0; k < obstacleNow.Num(); k++)
				{
					if (obstacleNow[k].Id == -2)
					{
						continue;
					}
					float distance = FVector::Distance(obstacleLast[j].Position, obstacleNow[k].Position);
					if (distance < mObstacleThreshold && distance < minDistance)
					{
						minDistance = distance;
						matchIndexLast = j;
						matchIndexNow = k;
					}
				}
			}
			if (matchIndexLast != -1 && obstacleLast[matchIndexLast].Id != -1) //if find same obstacle
			{
				obstacle[matchIndexNow].Id = obstacleLast[matchIndexLast].Id;
				obstacle[matchIndexNow].Duration = obstacleLast[matchIndexLast].Duration;
				obstacle[matchIndexNow].Duration += mSleepInterval;
				obstacleLast[matchIndexLast].Id = -2;//filter compared id
				obstacleNow[matchIndexNow].Id = -2;//filter compared id
			}
			else
			{
				for (int l = 0; l < obstacleLast.Num(); l++)
				{
					if (obstacleLast[l].Id != -2)
					{
					AddObstacleUp(obstacleLast[l]);
					if (obstacleLast[l].Id > maxId)
					{
						maxId = obstacleLast[l].Id;
					}
					}
				}
				break;
			}
		}

		for (int i = 0; i < obstacle.Num(); i++)
		{
			if (obstacle[i].Id == -1)
			{
				// give it a new Id
				int newId = -1;
				newId = ++maxId;
				obstacle[i].Id = newId;
				AddObstacleDown(obstacle[i]);
			}
		}
		obstacleLast = obstacle;
		std::lock_guard<std::mutex> lock(mMutexObstacle);
		mObstacle = obstacle;
	}
}

void AURG_04LX_UG01_Handler::AddObstacleDown(FObstacle aObstacleDown)
{
	std::lock_guard<std::mutex> lock(mMutexObstacleDown);
	mObstacleDown.Add(aObstacleDown);
}

void AURG_04LX_UG01_Handler::AddObstacleUp(FObstacle aObstacleUp)
{
	std::lock_guard<std::mutex> lock(mMutexObstacleUp);
	mObstacleUp.Add(aObstacleUp);
}

void AURG_04LX_UG01_Handler::Clear()
{
	mURGWrapper = new URG_04LX_UG01_Wrapper();
	mFShutdownFliter = false;

	/*mIsInitialize = false;
	mMaxMeasurementDistance = 0;
	mMinMeasurementDistance = 0;
	mDegreeArray.Empty();
	mOriginDegreeAndDistance.Empty();
	mModifiedDegreeAndDistance.Empty();
	mEnvironmentInfo.Empty();*/
}

void AURG_04LX_UG01_Handler::Shutdown()
{
	if (mURGWrapper != nullptr)
	{
		if (mURGResetWorker != nullptr) {
			mURGResetWorker->Stop();
			delete mURGResetWorker;
			mURGResetWorker = nullptr;
		}

		//LoopWorker
		mURGWrapper->ShutDown();
		if (mURGLoopWorker != nullptr) {
			mURGLoopWorker->Stop();
			delete mURGLoopWorker;
			mURGLoopWorker = nullptr;
		}
		//Fliter
		mFShutdownFliter = true;
		if (mURGFilter != nullptr) {
			mURGFilter->Stop();
			delete mURGFilter;
			mURGFilter = nullptr;
		}
		mURGWrapper->StopURG();
		mURGWrapper->Clear();
		delete mURGWrapper;
		mURGWrapper = nullptr;
	}
}

void AURG_04LX_UG01_Handler::AddInsideObstacle(FString Degree, bool Inside)
{
	std::lock_guard<std::mutex> lock(mMutexInsideObstacle);
	mInsideObstacle.Add(Degree, Inside);
}

TMap<FString, float> AURG_04LX_UG01_Handler::GetModifiedDegreeAndDistance()
{
	TMap<FString, float> rtn;
	std::lock_guard<std::mutex> lock(mMutexModifiedDegreeAndDistance);
	rtn = mModifiedDegreeAndDistance;
	return rtn;
}

TMap<FString, float> AURG_04LX_UG01_Handler::GetEnvironmentInfo()
{
	TMap<FString, float> rtn;
	std::lock_guard<std::mutex> lock(mMutexEnvironmentInfo);
	rtn = mEnvironmentInfo;
	return rtn;
}

//LoopWorker

URGLoopWorker::URGLoopWorker(URG_04LX_UG01_Wrapper* TargetURGWrapper)
{
	mURGWrapper = TargetURGWrapper;
	mThreadWorker = nullptr;
}

URGLoopWorker::~URGLoopWorker()
{
}

bool URGLoopWorker::Init(FString ThreadName)
{
	mThreadWorker = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);

	return true;
}

uint32 URGLoopWorker::Run()
{
	mURGWrapper->URGLiveLoop();

	return 1;
}

void URGLoopWorker::Stop()
{
	WaitForCompletion();
}

void URGLoopWorker::WaitForCompletion()
{
	if (mThreadWorker != nullptr) {
		mThreadWorker->WaitForCompletion();
	}
}

//Fliter

URGFilter::URGFilter(AURG_04LX_UG01_Handler* TargetURGHandler)
{
	mURGHandler = TargetURGHandler;
	mThreadWorker = nullptr;
}

URGFilter::~URGFilter()
{
}

bool URGFilter::Init(FString ThreadName)
{
	mThreadWorker = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);

	return true;
}

uint32 URGFilter::Run()
{
	mURGHandler->URGFilterLiveLoop();

	return 1;
}

void URGFilter::Stop()
{
	WaitForCompletion();
}

void URGFilter::WaitForCompletion()
{
	if (mThreadWorker != nullptr) {
		mThreadWorker->WaitForCompletion();
	}
}

URGResetWorker::URGResetWorker(AURG_04LX_UG01_Handler* TargetURGHandler, float aScanRangeMin, float aScanRangeMax, qrk::Lidar::connection_type_t aConnectionType, FString aDevice_or_ip_name, int aBaudrate_or_port_number)
{
	mURGHandler = TargetURGHandler;
	mScanRangeMin = aScanRangeMin;
	mScanRangeMax = aScanRangeMax;
	mConnectionType = aConnectionType;
	mDevice_or_ip_name = aDevice_or_ip_name;
	mBaudrate_or_port_number = aBaudrate_or_port_number;
	mThreadWorker = nullptr;

	mURGHandler->Shutdown();
	mURGHandler->Clear();
}

URGResetWorker::~URGResetWorker()
{
}

bool URGResetWorker::Init(FString ThreadName)
{
	mThreadWorker = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);

	return false;
}

uint32 URGResetWorker::Run()
{
	bool result = mURGHandler->mURGWrapper->OpenURG(mScanRangeMin, mScanRangeMax, mConnectionType, TCHAR_TO_ANSI(*mDevice_or_ip_name), mBaudrate_or_port_number);
	if (result) {
		mURGHandler->StartURGLoopWorker();
		mURGHandler->StartURGFilter();
	}

	return result;
}

void URGResetWorker::Stop()
{
	WaitForCompletion();
}

void URGResetWorker::WaitForCompletion()
{
	if (mThreadWorker != nullptr) {
		mThreadWorker->WaitForCompletion();
	}
}
