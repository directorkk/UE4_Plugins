// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WindowsSocket/Private/SocketHandlerBase.h"

//#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <future>
#include <mutex>
#include <map>


/**
 * 
 */
class WINDOWSSOCKET_API TCPSocket : public SocketHandlerBase
{
public:
	TCPSocket(WORD SockVersion = MAKEWORD(2, 2));
	virtual ~TCPSocket();

	bool InitServer(unsigned short LocalPort);
	bool StartServer(int MaxListenBacklog = 10);

	bool InitClient(std::string TargetIP, unsigned short TargetPort);
	bool StartClient();
	void Send(std::string Message, std::vector<LinkInfo> Targets = std::vector<LinkInfo>());

	void SetPacketPattern(std::string Pattern);

	void Shutdown();
	bool IsAlive();

private:
	void ServerLiveLoop();
	void ClientLiveLoop();

	void Clear();

protected:
	virtual void ServerAcceptNewLink(LinkInfo Connection);
	virtual void ServerLoseLink(unsigned int Index);

	virtual void ClientLoseLink();

	virtual void RecvDataCallback(DataPacket packet);
	virtual void PackMessage(std::string Message, char** ppData, long long& Len);
	virtual void UnpackMessage(LinkInfo* pSourceInfo, char* Pattern, int PatternLen);

private:
	void ServerRecvPacket(LinkInfo* pSourceInfo, BYTE *Data, unsigned int BufferSize);
	void ClientRecvPacket(BYTE *Data, unsigned int BufferSize);

	void SendToServer(char* Data, long long DataLen);
	void SendToClient(char* Data, long long DataLen, std::vector<LinkInfo> Targets);
	void SendImplement(SOCKET s, const char* buf, long long len, int flags);

	std::vector<int> DatacmpIndex(char* Data, int DataSize, char* Pattern, int PatternSize, int MaxCount = 0);

	std::map<LinkInfo*, std::vector<DataPacket>> mMapProcessedPacket;
	std::string mPacketPattern;

protected:
	std::vector<BYTE> mAryRecvData;
	std::vector<LinkInfo> mLinkedClients;
	LinkInfo mLinkedServer;

	FThreadSafeBool mFShutdown;
	FThreadSafeBool mFIsThreadAlive;
	std::atomic<bool> mFShutdownCheck;
	std::thread mpThreadServer;
	std::thread mpThreadClient;
	std::mutex mMutexThreadServerRecv;
	std::mutex mMutexThreadClientRecv;
	short mSocketType;
};
