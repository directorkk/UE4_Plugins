// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UDPSocket.h"

#include <mutex>

#include "GameFramework/Actor.h"
#include "OrientationDeviceHandlerBase.generated.h"


USTRUCT(BlueprintType)
struct FTriggerQuatData {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FRotator Rotator;

	FTriggerQuatData() {
		Id = 0;
		Rotator = FRotator();
	}
};
USTRUCT(BlueprintType)
struct FTriggerOrientationData {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector Orientation;

	FTriggerOrientationData() {
		Id = 0;
		Orientation = FVector();
	}
};
USTRUCT(BlueprintType)
struct FTriggerStickData {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector2D Stick;

	FTriggerStickData() {
		Id = 0;
		Stick = FVector2D();
	}
};
USTRUCT(BlueprintType)
struct FTriggerPressedStateData {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int ButtonId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int PressedState;

	FTriggerPressedStateData() {
		Id = 0;
		PressedState = -1;
	}
};

struct ScheduledPacket {
	bool State;
	DataPacket Packet;

	ScheduledPacket() {
		State = false;
		memset(&Packet, 0, sizeof(DataPacket));
	}

	bool IsUpdateNeeded() {
		return State == false && Packet.Len != 0;
	}
};


UCLASS()
class ORIENTATIONDEVICE_API AOrientationDeviceHandlerBase : public AActor, public UDPSocket
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrientationDeviceHandlerBase();

	UFUNCTION(BlueprintCallable)
		void SetupConnectionInfo(int Port);
	UFUNCTION(BlueprintCallable)
		void StartupServer();

	UFUNCTION(BlueprintImplementableEvent)
		void PrintStringEditor(const FString &Message);

	UFUNCTION(BlueprintImplementableEvent)
		void RecvDataCallback_Quat_Implement(const FTriggerQuatData &QuatData);
	UFUNCTION(BlueprintImplementableEvent)
		void RecvDataCallback_Orientation_Implement(const FTriggerOrientationData &OrientationData);
	UFUNCTION(BlueprintImplementableEvent)
		void RecvDataCallback_Calibrate_Implement(const FTriggerQuatData &QuatData);
	UFUNCTION(BlueprintImplementableEvent)
		void RecvDataCallback_PressedState_Implement(const FTriggerPressedStateData &PressedStateData);
	UFUNCTION(BlueprintImplementableEvent)
		void RecvDataCallback_Stick_Implement(const FTriggerStickData &OrientationData);
	UFUNCTION(BlueprintImplementableEvent)
		void ServerLoseLink_Implement(const int Id);

private:
	ScheduledPacket mQuatPacket;
	ScheduledPacket mOrientationPacket;
	ScheduledPacket mCalibratePacket;
	//ScheduledPacket mStickPacket;
	std::vector<ScheduledPacket> mStickPacket;
	//ScheduledPacket mPressedStatePacket;
	std::vector<ScheduledPacket> mPressedStatePacket;
	std::vector<int> mAryDisconnection;

	std::mutex mMutexProcessScheduledPacket;

	FTriggerQuatData ParseDataPacket_Quat(DataPacket ReceivedDataPacket);
	FTriggerOrientationData ParseDataPacket_OrientationData(DataPacket ReceivedDataPacket);
	FTriggerStickData ParseDataPacket_StickData(DataPacket ReceivedDataPacket);
	FTriggerPressedStateData ParseDataPacket_PressedStateData(DataPacket ReceivedDataPacket);

	int mLocalPort;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void RecvDataCallback(DataPacket Packet) override;
	//virtual void ServerLoseLink(unsigned int Index) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
