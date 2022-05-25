// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketHandlerBase.h"
using namespace WindowsSocket;

SocketHandlerBase::SocketHandlerBase(WORD sockVersion)
{
	if (WSAStartup(sockVersion, &mWsaData) != 0) {
		WSACleanup();

		OutputDebugString(L"ERROR: Socket version is not supported");
		mFSuccess = false;

		return;
	}
	mFSuccess = true;
}

SocketHandlerBase::~SocketHandlerBase()
{
	if (mFSuccess) {
		WSACleanup();
	}
}

void SocketHandlerBase::Shutdown()
{

}

void SocketHandlerBase::ServerLiveLoop()
{

}

void SocketHandlerBase::ClientLiveLoop()
{

}


void SocketHandlerBase::RecvDataCallback(DataPacket packet)
{

}

/*
* ServerLiveLoopWorker
*/
ServerLiveLoopWorker::ServerLiveLoopWorker(SocketHandlerBase* TargetSocketHandler)
{
	mSocketHandler = TargetSocketHandler;
	mThreadWorker = nullptr;
}

ServerLiveLoopWorker::~ServerLiveLoopWorker()
{

}

bool ServerLiveLoopWorker::Init(FString ThreadName)
{
	mThreadWorker = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);

	return true;
}

uint32 ServerLiveLoopWorker::Run()
{
	mSocketHandler->ServerLiveLoop();

	return 1;
}

void ServerLiveLoopWorker::Stop()
{
	WaitForCompletion();
}

void ServerLiveLoopWorker::WaitForCompletion()
{
	if (mThreadWorker != nullptr) {
		mThreadWorker->WaitForCompletion();
	}
}


/*
* ClientLiveLoopWorker
*/
ClientLiveLoopWorker::ClientLiveLoopWorker(SocketHandlerBase* TargetSocketHandler)
{
	mSocketHandler = TargetSocketHandler;
	mThreadWorker = nullptr;
}

ClientLiveLoopWorker::~ClientLiveLoopWorker()
{

}

bool ClientLiveLoopWorker::Init(FString ThreadName)
{
	mThreadWorker = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);

	return true;
}

uint32 ClientLiveLoopWorker::Run()
{
	mSocketHandler->ClientLiveLoop();

	return 1;
}

void ClientLiveLoopWorker::Stop()
{
	WaitForCompletion();
}

void ClientLiveLoopWorker::WaitForCompletion()
{
	if (mThreadWorker != nullptr) {
		mThreadWorker->WaitForCompletion();
	}
}

