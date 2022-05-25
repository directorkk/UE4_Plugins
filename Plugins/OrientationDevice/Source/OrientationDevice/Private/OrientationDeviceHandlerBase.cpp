// Fill out your copyright notice in the Description page of Project Settings.


#include "OrientationDeviceHandlerBase.h"

// Sets default values
AOrientationDeviceHandlerBase::AOrientationDeviceHandlerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mLocalPort = 0;

}

void AOrientationDeviceHandlerBase::SetupConnectionInfo(int Port)
{
	mLocalPort = Port;
}

void AOrientationDeviceHandlerBase::StartupServer()
{
	InitServer(mLocalPort);
	StartServer();
}

// Called when the game starts or when spawned
void AOrientationDeviceHandlerBase::BeginPlay()
{
	Super::BeginPlay();

}

void AOrientationDeviceHandlerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Shutdown();
}

void AOrientationDeviceHandlerBase::RecvDataCallback(DataPacket Packet)
{
	FString msg = Packet.Data.data();

	//Util::OutputDebugMessage(msg);
	PrintStringEditor(msg);

	ScheduledPacket scheduledPacket;
	scheduledPacket.Packet = Packet;

	std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
	if (msg.ToLower().Contains(FString("QUAT").ToLower())) {
		mQuatPacket = scheduledPacket;
	}
	if (msg.ToLower().Contains(FString("ORIENTATION").ToLower())) {
		mOrientationPacket = scheduledPacket;
	}
	if (msg.ToLower().Contains(FString("CALIBRATE").ToLower())) {
		mCalibratePacket = scheduledPacket;
	}
	if (msg.ToLower().Contains(FString("STICK").ToLower())) {
		mStickPacket.push_back(scheduledPacket);
	}
	if (msg.ToLower().Contains(FString("PRESS").ToLower()) || msg.ToLower().Contains(FString("RELEASE").ToLower())) {
		mPressedStatePacket.push_back(scheduledPacket);
	}
}

FTriggerQuatData AOrientationDeviceHandlerBase::ParseDataPacket_Quat(DataPacket ReceivedDataPacket)
{
	FTriggerQuatData rtn;
	int id = -1;
	float values[4] = { 0 };

	FString msg = ReceivedDataPacket.Data.data();
	FString msgType, msgData;
	msg.Split("|", &msgType, &msgData);

	int counterSplit = 0;
	FString leftStr, rightStr;
	msgData.Split(";", &leftStr, &rightStr);
	id = FCString::Atoi(*leftStr);
	msgData = rightStr;
	msgData.Split(",", &leftStr, &rightStr);
	values[0] = FCString::Atof(*leftStr);
	msgData = rightStr;
	msgData.Split(",", &leftStr, &rightStr);
	values[1] = FCString::Atof(*leftStr);
	msgData = rightStr;
	msgData.Split(",", &leftStr, &rightStr);
	values[2] = FCString::Atof(*leftStr);
	values[3] = FCString::Atof(*rightStr);

	FQuat quat(values[1], values[2], values[3], values[0]);

	rtn.Id = id;
	rtn.Rotator = quat.Rotator();

	return rtn;
}

/*
void AOrientationDeviceHandlerBase::ServerLoseLink(unsigned int Index)
{
	std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);

	int id = mLinkedClients[Index].socket;
	mAryDisconnection.push_back(id);

	TCPSocket::ServerLoseLink(Index);
}*/

FTriggerOrientationData AOrientationDeviceHandlerBase::ParseDataPacket_OrientationData(DataPacket ReceivedDataPacket)
{
	FTriggerOrientationData rtn;
	float values[3] = { 0 };

	FString msg = ReceivedDataPacket.Data.data();
	FString msgType, msgData;
	msg.Split("|", &msgType, &msgData);

	int counterSplit = 0;
	FString leftStr, rightStr;
	msgData.Split(",", &leftStr, &rightStr);
	values[0] = FCString::Atof(*leftStr);
	msgData = rightStr;
	msgData.Split(",", &leftStr, &rightStr);
	values[1] = FCString::Atof(*leftStr);
	values[2] = FCString::Atof(*rightStr);

	rtn.Id = ReceivedDataPacket.Owner.socket;
	rtn.Orientation.X = values[0];
	rtn.Orientation.Y = values[1];
	rtn.Orientation.Z = values[2];

	return rtn;
}

FTriggerStickData AOrientationDeviceHandlerBase::ParseDataPacket_StickData(DataPacket ReceivedDataPacket)
{
	FTriggerStickData rtn;
	float values[3] = { 0 };

	FString msg = ReceivedDataPacket.Data.data();
	FString msgType, msgData;
	msg.Split("|", &msgType, &msgData);

	FString ConId, StrData;
	msgData.Split(";", &ConId, &StrData);

	int counterSplit = 0;
	FString leftStr, rightStr;
	StrData.Split(",", &leftStr, &rightStr);
	values[0] = FCString::Atof(*leftStr);
	values[1] = FCString::Atof(*rightStr);

	//rtn.Id = ReceivedDataPacket.Owner.socket;
	//rtn.Id = FCString::Atoi(*ConId);
	rtn.Id = FCString::Atoi(*ConId);
	rtn.Stick.X = values[0];
	rtn.Stick.Y = values[1];

	return rtn;
}

FTriggerPressedStateData AOrientationDeviceHandlerBase::ParseDataPacket_PressedStateData(DataPacket ReceivedDataPacket)
{
	FTriggerPressedStateData rtn;

	FString msg = ReceivedDataPacket.Data.data();
	FString msgType, msgData;
	msg.Split("|", &msgType, &msgData);

	FString ConId, StrData;
	msgData.Split(";", &ConId, &StrData);

	int pressedState = -1;
	if (msgType.Equals("PRESS")) {
		pressedState = 1;
	}
	if (msgType.Equals("RELEASE")) {
		pressedState = 0;
	}
	//ReceivedDataPacket.Owner.address.sin_port
	//rtn.Id = ReceivedDataPacket.Owner.socket;
	//rtn.Id = FCString::Atoi(*msgData) % 10;
	rtn.Id = FCString::Atoi(*ConId);
	rtn.ButtonId = FCString::Atoi(*StrData);
	rtn.PressedState = pressedState;

	return rtn;
}


// Called every frame
void AOrientationDeviceHandlerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mQuatPacket.IsUpdateNeeded()) {
		std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
		mQuatPacket.State = true;
		RecvDataCallback_Quat_Implement(ParseDataPacket_Quat(mQuatPacket.Packet));
	}
	if (mOrientationPacket.IsUpdateNeeded()) {
		std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
		mOrientationPacket.State = true;
		RecvDataCallback_Orientation_Implement(ParseDataPacket_OrientationData(mOrientationPacket.Packet));
	}
	if (mCalibratePacket.IsUpdateNeeded()) {
		std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
		mCalibratePacket.State = true;
		RecvDataCallback_Calibrate_Implement(ParseDataPacket_Quat(mCalibratePacket.Packet));
	}
	if (mPressedStatePacket.size() > 0)
	{
		for (int i = 0; i < mPressedStatePacket.size(); i++)
		{
			std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
			ScheduledPacket *pScheduledPacket = &mPressedStatePacket.at(i);
			pScheduledPacket->State = true;
			RecvDataCallback_PressedState_Implement(ParseDataPacket_PressedStateData(pScheduledPacket->Packet));
		}
		mPressedStatePacket.clear();
	}
	/*if (mPressedStatePacket.IsUpdateNeeded()) {
		std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
		mPressedStatePacket.State = true;
		RecvDataCallback_PressedState_Implement(ParseDataPacket_PressedStateData(mPressedStatePacket.Packet));
	}*/
	if (mStickPacket.size() > 0)
	{
		for (int i = 0; i < mStickPacket.size(); i++)
		{
			std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
			ScheduledPacket *pScheduledPacket = &mStickPacket.at(i);
			pScheduledPacket->State = true;
			RecvDataCallback_Stick_Implement(ParseDataPacket_StickData(pScheduledPacket->Packet));
		}
		mStickPacket.clear();
	}
	/*if (mStickPacket.IsUpdateNeeded()) {
		std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
		mStickPacket.State = true;
		RecvDataCallback_Stick_Implement(ParseDataPacket_StickData(mStickPacket.Packet));
	}*/
	/*if (mAryDisconnection.size() != 0) {
		std::lock_guard<std::mutex> lock(mMutexProcessScheduledPacket);
		for (int i = 0; i < mAryDisconnection.size(); i++) {
			ServerLoseLink_Implement(mAryDisconnection.at(i));
		}
		mAryDisconnection.clear();
	}*/
}

