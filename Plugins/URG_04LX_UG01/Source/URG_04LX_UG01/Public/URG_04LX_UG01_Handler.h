// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/CString.h"
#include "URG_04LX_UG01/Private/URG_04LX_UG01_Wrapper.h"
#include "GameFramework/Actor.h"
#include "URG_04LX_UG01_Handler.generated.h"

UENUM(BlueprintType)
enum ConnectionType
{
	ConnectionType_Serial    UMETA(DisplayName = "Serial"),
	ConnectionType_Ethernet  UMETA(DisplayName = "Ethernet")
};

USTRUCT(BlueprintType)
struct FTarget
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
		AActor* TargetActor;
	UPROPERTY(BlueprintReadWrite)
		float InteractionToleranceDistance;
	UPROPERTY(BlueprintReadWrite)
		float InteractionToleranceAngle;
	UPROPERTY(BlueprintReadWrite)
		bool Triggered;
	UPROPERTY(BlueprintReadWrite)
		bool InRange;
	UPROPERTY(BlueprintReadWrite)
		int Id;

	FTarget()
	{
		TargetActor = nullptr;
		InteractionToleranceDistance = 10.0f;
		InteractionToleranceAngle = 2.0f;
		Triggered = false;
		InRange = false;
		Id = -1;
	}
};


USTRUCT(BlueprintType)
struct FObstacle
{
	GENERATED_BODY()
		UPROPERTY(BlueprintReadWrite)
		float DegreeMin;
	UPROPERTY(BlueprintReadWrite)
		float DegreeMax;
	UPROPERTY(BlueprintReadWrite)
		float AverageDistance;
	UPROPERTY(BlueprintReadWrite)
		FVector Position;
	UPROPERTY(BlueprintReadWrite)
		float Duration;
	UPROPERTY(BlueprintReadWrite)
		bool Valid;
	UPROPERTY(BlueprintReadWrite)
		int Id;

	FObstacle()
	{
		DegreeMin = DegreeMax = AverageDistance = Duration = 0.0f;
		Position = FVector::ZeroVector;
		Valid = false;
		Id = -1;
	}
};

class URGFilter;
class URGLoopWorker;
class URGResetWorker;

UCLASS()
class URG_04LX_UG01_API AURG_04LX_UG01_Handler : public AActor
{
	GENERATED_BODY()
	
	friend class URGLoopWorker;
	//friend class Filter;

public:	
	// Sets default values for this actor's properties
	AURG_04LX_UG01_Handler();
	URG_04LX_UG01_Wrapper* mURGWrapper;
	URGLoopWorker* mURGLoopWorker;
	URGFilter* mURGFilter;
	URGResetWorker *mURGResetWorker;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	bool mIsURGReady = false;
	float mTimerReset = 0;
	float mIntervalReset = 15;	// second

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//variable
	const float mDegPerRad = 57.3f;
	const long mSleepInterval = 10;

	UPROPERTY(BlueprintReadWrite)
		bool mIsInitialize;
	UPROPERTY(BlueprintReadWrite)
		float mDetectDistanceMax;
	UPROPERTY(BlueprintReadWrite)
		float mDetectDistanceMin;
	UPROPERTY(BlueprintReadWrite)
		float mObstacleThreshold;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> mDegreeArray;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, float> mOriginDegreeAndDistance;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, float> mModifiedDegreeAndDistance;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, float> mEnvironmentInfo;

	//function
	UFUNCTION(BlueprintCallable)
		void OpenURG(float aScanRangeMin, float aScanRangeMax, float aDetectDistanceMin, float aDetectDistanceMax, ConnectionType aConnectionType, FString aDevice_or_ip_name, int aBaudrate_or_port_number, bool& oIsOpen);
	UFUNCTION(BlueprintCallable)
		void OpenURGWithoutEnvironmentSetting(float aScanRangeMin, float aScanRangeMax, ConnectionType aConnectionType, FString aDevice_or_ip_name, int aBaudrate_or_port_number, bool& oIsOpen);
	UFUNCTION(BlueprintCallable)
		void StopURG();
	UFUNCTION(BlueprintCallable)
		void GetDegreeAndDistance(TMap<FString, float>& oDegreeAndDistance);
	UFUNCTION(BlueprintCallable)
		void SetScanRange(int aMinDegree, int aMaxDegree);
	UFUNCTION(BlueprintCallable)
		void ModifyDegreeAndDistanceMap(TMap<FString, float> aOriginMap, TMap<FString, float>& oResultMap);
	UFUNCTION(BlueprintCallable)
		void GetInteractionResult(TMap<FString, float> aDegreeAndDistance, TArray<FTarget> aInputTarget, TArray<FTarget>& oResultTarget);
	UFUNCTION(BlueprintCallable)
		void GetObstacle(TMap<FString, float> aDegreeAndDistance, TArray<FObstacle>& oObstacle);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetFilteredObstacle(TArray<FObstacle>& oObstacle);
	UFUNCTION(BlueprintCallable)
		void SaveEnvironmentInfo(TMap<FString, float> aEnvironmentInfo);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetInsideObstacle(TMap<FString, bool>& oInsideObstacle);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetObstacleDown(TArray<FObstacle>& oObstacleDown);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetObstacleUp(TArray<FObstacle>& oObstacleUp);

	//URGHandler
	UFUNCTION(BlueprintCallable)
		void Clear();
	UFUNCTION(BlueprintCallable)
		void Shutdown();
	void AddInsideObstacle(FString Degree, bool Inside);
	TMap<FString, float> GetModifiedDegreeAndDistance();
	TMap<FString, float> GetEnvironmentInfo();

	//URGLoopWorker
	void StartURGLoopWorker();
	void StartURGResetWorker();

	//URGFilter
	void StartURGFilter();
	void URGFilterLiveLoop();

private:
	FThreadSafeBool mFShutdownFliter;
	std::mutex mMutexObstacle;
	std::mutex mMutexInsideObstacle;
	std::mutex mMutexModifiedDegreeAndDistance;
	std::mutex mMutexEnvironmentInfo;
	std::mutex mMutexObstacleDown;
	std::mutex mMutexObstacleUp;
	TArray<FObstacle> mObstacle;
	TArray<FObstacle> mObstacleDown;
	TArray<FObstacle> mObstacleUp;
	TMap<FString, bool> mInsideObstacle;
	
	void AddObstacleDown(FObstacle aObstacleDown);
	void AddObstacleUp(FObstacle aObstacleUp);

	friend class URGResetWorker;
};

class URGResetWorker : public FRunnable {
public:
	URGResetWorker(AURG_04LX_UG01_Handler* TargetURGHandler, float aScanRangeMin, float aScanRangeMax, qrk::Lidar::connection_type_t aConnectionType, FString aDevice_or_ip_name, int aBaudrate_or_port_number);
	~URGResetWorker();

	//FRunnable interface.
	virtual bool Init(FString ThreadName);
	virtual uint32 Run();
	virtual void Stop();

private:
	AURG_04LX_UG01_Handler* mURGHandler;
	float mScanRangeMin;
	float mScanRangeMax;
	qrk::Lidar::connection_type_t mConnectionType;
	FString mDevice_or_ip_name;
	int mBaudrate_or_port_number;

	//Thread to run the worker FRunnable on
	FRunnableThread* mThreadWorker;
	void WaitForCompletion();
};

class URGLoopWorker : public FRunnable {
public:
	URGLoopWorker(URG_04LX_UG01_Wrapper* TargetURGWrapper);
	~URGLoopWorker();

	//FRunnable interface.
	virtual bool Init(FString ThreadName);
	virtual uint32 Run();
	virtual void Stop();

private:
	URG_04LX_UG01_Wrapper* mURGWrapper;

	//Thread to run the worker FRunnable on
	FRunnableThread* mThreadWorker;
	void WaitForCompletion();
};

class URGFilter : public FRunnable {
public:
	URGFilter(AURG_04LX_UG01_Handler* TargetURGHandler);
	~URGFilter();

	//FRunnable interface.
	virtual bool Init(FString ThreadName);
	virtual uint32 Run();
	virtual void Stop();

private:
	AURG_04LX_UG01_Handler* mURGHandler;

	//Thread to run the worker FRunnable on
	FRunnableThread* mThreadWorker;
	void WaitForCompletion();
};