// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Windows/WindowsWindow.h"

#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "HAL/RunnableThread.h"


// winsock definition
// TODO: fix deprecated functions
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef __cplusplus
extern "C" {
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#ifdef __cplusplus
}
#endif

#ifndef __WGSOCKET_H__  
#define __WGSOCKET_H__  
#ifdef WIN32  
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")  
#else  
#include <iostream>
#define SOCKET_ERROR -1  
#define INVALID_SOCKET -1  
#define SHUT_RD 0 /* No more receptions.  */  
#define SHUT_WR 1 /* No more transmissions.  */  
#define SHUT_RDWR 2 /* No more receptions or transmissions.  */  
#endif 
#endif

#include <vector>
#include <string>
#include <sstream>

#include "Util.h"
using namespace WindowsSocket;


#define _SOCKET_PLUGIN_MAX_BUFFER 65535

enum {
	_SOCKET_PLUGIN_ADDRESS_IP = 0,
	_SOCKET_PLUGIN_ADDRESS_PORT,
	_SOCKET_PLUGIN_ADDRESS_IP_AND_PORT,
	_SOCKET_PLUGIN_TYPE_NONE,
	_SOCKET_PLUGIN_TYPE_CLIENT,
	_SOCKET_PLUGIN_TYPE_SERVER,
};

struct LinkInfo;
struct DataPacket;
class ServerLiveLoopWorker;
class ClientLiveLoopWorker;
class RecvDataCallbackWorker;

/**
 *
 */
class SocketHandlerBase
{
	friend class ServerLiveLoopWorker;
	friend class ClientLiveLoopWorker;

public:
	SocketHandlerBase(WORD sockVersion);
	~SocketHandlerBase();

	virtual void Shutdown();

protected:
	virtual void ServerLiveLoop();
	virtual void ClientLiveLoop();
	virtual void RecvDataCallback(DataPacket packet);

	bool mFSuccess;

	WSADATA mWsaData;
	sockaddr_in mLocalAddr;
	sockaddr_in mExternalAddr;
	SOCKET mSocket;

	ServerLiveLoopWorker* mServerLiveLoopWorker;
	ClientLiveLoopWorker* mClientLiveLoopWorker;
};


struct LinkInfo {
	SOCKET socket;
	sockaddr_in address;

	LinkInfo() {
		socket = 0;
		memset(&address, 0, sizeof(sockaddr_in));
	}

	LinkInfo(SOCKET _socket, sockaddr_in _address) {
		socket = _socket;
		address = _address;
	}

	std::string GetIPAddress(int Type = _SOCKET_PLUGIN_ADDRESS_IP_AND_PORT) {
		std::ostringstream oss;
		wchar_t tmpWsIp[16] = { 0 };

		switch (Type)
		{
		case _SOCKET_PLUGIN_ADDRESS_IP:
			InetNtop(address.sin_family, &address.sin_addr.S_un.S_addr, tmpWsIp, sizeof(tmpWsIp));
			oss << Util::wstos(tmpWsIp);
			break;
		case _SOCKET_PLUGIN_ADDRESS_PORT:
			oss << ntohs(address.sin_port);
			break;
		case _SOCKET_PLUGIN_ADDRESS_IP_AND_PORT:
			InetNtop(address.sin_family, &address.sin_addr.S_un.S_addr, tmpWsIp, sizeof(tmpWsIp));
			oss << Util::wstos(tmpWsIp) << ":" << ntohs(address.sin_port);
			break;
		default:
			oss << "0.0.0.0:0";
			break;
		}

		return oss.str();
	}

	bool operator==(const LinkInfo AnotherLinkInfo) {
		return socket == AnotherLinkInfo.socket;
	}
	bool operator!=(const LinkInfo AnotherLinkInfo) {
		return socket != AnotherLinkInfo.socket;
	}
};
struct DataPacket {
	//SOCKET Owner;
	LinkInfo Owner;
	std::vector<char> Data;
	long long Len;
	long long RecvCounter;

	DataPacket() {
		Owner = LinkInfo();
		Len = RecvCounter = 0;
	}
};

class ServerLiveLoopWorker : public FRunnable {
public:
	ServerLiveLoopWorker(SocketHandlerBase* TargetSocketHandler);
	~ServerLiveLoopWorker();

	//FRunnable interface.
	virtual bool Init(FString ThreadName);
	virtual uint32 Run();
	virtual void Stop();


private:
	SocketHandlerBase* mSocketHandler;

	//Thread to run the worker FRunnable on
	FRunnableThread* mThreadWorker;
	void WaitForCompletion();
};

class ClientLiveLoopWorker : public FRunnable {
public:
	ClientLiveLoopWorker(SocketHandlerBase* TargetSocketHandler);
	~ClientLiveLoopWorker();

	//FRunnable interface.
	virtual bool Init(FString ThreadName);
	virtual uint32 Run();
	virtual void Stop();

private:
	SocketHandlerBase* mSocketHandler;

	//Thread to run the worker FRunnable on
	FRunnableThread* mThreadWorker;
	void WaitForCompletion();
};
