// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPSocket.h"
using namespace WindowsSocket;


TCPSocket::TCPSocket(WORD SockVersion) : SocketHandlerBase(SockVersion)
{
	Clear();

	mPacketPattern = "DATA_START";
}

TCPSocket::~TCPSocket()
{

}

bool TCPSocket::InitClient(std::string TargetIP, unsigned short TargetPort)
{
	if (mSocketType != _SOCKET_PLUGIN_TYPE_NONE) {
		Util::OutputDebugMessage("ERROR: Socket had been init, call \'Release()\' first");
		return false;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket == INVALID_SOCKET) {
		Util::OutputDebugMessage("ERROR: Fail to create TCP socket");
	}

	hostent *hostinfo = gethostbyname(TargetIP.c_str());
	if (!hostinfo) {
		Util::OutputDebugMessage("ERROR: Fail to create host setting context");
		return false;
	}

	mExternalAddr.sin_family = AF_INET;
	mExternalAddr.sin_addr = *(struct in_addr *) hostinfo->h_addr;
	mExternalAddr.sin_port = htons(TargetPort);

	mSocketType = _SOCKET_PLUGIN_TYPE_CLIENT;

	return true;
}

bool TCPSocket::InitServer(unsigned short LocalPort)
{
	if (mSocketType != _SOCKET_PLUGIN_TYPE_NONE) {
		Util::OutputDebugMessage("ERROR: Socket had been init, call \'Release()\' first");
		return false;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket == INVALID_SOCKET) {
		Util::OutputDebugMessage("ERROR: Fail to create TCP socket");
	}

	mLocalAddr.sin_family = AF_INET;
	mLocalAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	mLocalAddr.sin_port = htons(LocalPort);

	mSocketType = _SOCKET_PLUGIN_TYPE_SERVER;

	return true;
}

bool TCPSocket::StartServer(int MaxListenBacklog /*= 10*/)
{
	if (bind(mSocket, (sockaddr*)&mLocalAddr, sizeof(mLocalAddr)) != 0) {
		Util::OutputDebugMessage("ERROR: Fail to bind");
		return false;
	}

	int opt = true;
	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		Util::OutputDebugMessage("ERROR: Fail to setsockopt");
		return false;
	}

	if (listen(mSocket, MaxListenBacklog) != 0) {
		Util::OutputDebugMessage("ERROR: Fail to listen");
		return false;
	}

	Util::OutputDebugMessage("Waiting for connection...");
	//mpThreadServer = std::thread(&TCPSocket::ServerLiveLoop, this);
	mServerLiveLoopWorker = new ServerLiveLoopWorker(this);
	mServerLiveLoopWorker->Init("TCPSocketServerLiveLoop");

	return true;
}

void TCPSocket::ServerLiveLoop()
{
	if (mSocketType != _SOCKET_PLUGIN_TYPE_SERVER) {
		Util::OutputDebugMessage("ERROR: This object is not a server");
		return;
	}

	mFIsThreadAlive = true;

	fd_set readfd;
	SOCKET maxfd, fd;

	int activity = 0;
	timeval timeout;
	timeout.tv_usec = 50000;
	std::ostringstream oss;
	int count = 0;
	int addrlen = sizeof(mLocalAddr);
	BYTE *recvbuffer = new BYTE[_SOCKET_PLUGIN_MAX_BUFFER];

	while (!mFShutdown) {
		FD_ZERO(&readfd);
		FD_SET(mSocket, &readfd);
		maxfd = mSocket;

		// put multi clients in readfd set for select
		for (unsigned int i = 0; i < mLinkedClients.size(); i++) {
			fd = mLinkedClients[i].socket;

			if (fd > 0) {
				FD_SET(fd, &readfd);
			}
			if (fd > maxfd) {
				maxfd = fd;
			}
		}

		// max file descripter+1, readset, writeset, expectset, timeout
		activity = select(maxfd + 1, &readfd, NULL, NULL, &timeout);
		if ((activity < 0) && (errno != EINTR)) {
			Util::OutputDebugMessage("Select error");
		}

		if (FD_ISSET(mSocket, &readfd)) {
			LinkInfo connection;
			if ((connection.socket = accept(mSocket, (sockaddr*)&connection.address, &addrlen)) < 0) {
				Util::OutputDebugMessage("Accept error");
				continue;
			}

			mLinkedClients.push_back(connection);

			ServerAcceptNewLink(connection);
		}

		for (unsigned int i = 0; i < mLinkedClients.size(); i++) {
			fd = mLinkedClients[i].socket;

			if (FD_ISSET(fd, &readfd)) {		// a client send something
				ZeroMemory(recvbuffer, _SOCKET_PLUGIN_MAX_BUFFER);
				int valread = 0;	// read how many data
				if ((valread = recv(fd, (char *)recvbuffer, _SOCKET_PLUGIN_MAX_BUFFER, 0)) <= 0) {
					// someone disconnected
					ServerLoseLink(i);
				}
				else {
					std::lock_guard<std::mutex> lock(mMutexThreadServerRecv);
					ServerRecvPacket(&mLinkedClients[i], recvbuffer, valread);
				}
			}
		}
	}

	delete recvbuffer;
	mFShutdownCheck = true;
	Util::OutputDebugMessage("Server shutdown");
}

void TCPSocket::ServerAcceptNewLink(LinkInfo Connection)
{
	std::ostringstream oss;
	oss << "New connection, fd: " << Connection.socket
		<< ", IP: " << Connection.GetIPAddress(_SOCKET_PLUGIN_ADDRESS_IP)
		<< ", port: " << Connection.GetIPAddress(_SOCKET_PLUGIN_ADDRESS_PORT) << std::endl;
	Util::OutputDebugMessage(oss.str().c_str());

	oss.clear();
	oss << Connection.socket << std::endl;
	std::string sendbuffer = oss.str();
	send(Connection.socket, sendbuffer.c_str(), sendbuffer.length(), 0);

	oss.clear();
	oss << "len: " << sendbuffer.length() << '\t' << "send: " << sendbuffer << std::endl;
	Util::OutputDebugMessage(oss.str().c_str());
}

void TCPSocket::ServerLoseLink(unsigned int Index)
{
	std::ostringstream oss;
	oss << "Lose connection, fd: " << mLinkedClients[Index].socket
		<< ", IP: " << mLinkedClients[Index].GetIPAddress(_SOCKET_PLUGIN_ADDRESS_IP)
		<< ", port: " << mLinkedClients[Index].GetIPAddress(_SOCKET_PLUGIN_ADDRESS_PORT) << std::endl;
	Util::OutputDebugMessage(oss.str().c_str());

	shutdown(mLinkedClients[Index].socket, SD_SEND);
	mLinkedClients.erase(mLinkedClients.begin() + Index);
}

void TCPSocket::ServerRecvPacket(LinkInfo* pSourceInfo, BYTE *Data, unsigned int BufferSize)
{
	mAryRecvData.insert(mAryRecvData.end(), Data, Data + BufferSize);

	std::string patternStr = mPacketPattern;
	int patternLen = patternStr.length();
	char* pattern = new char[patternLen];
	memcpy_s(pattern, patternLen, patternStr.c_str(), patternLen);

	UnpackMessage(pSourceInfo, pattern, patternLen);

	delete pattern;
}

bool TCPSocket::StartClient()
{
	if (mSocketType != _SOCKET_PLUGIN_TYPE_CLIENT) {
		Util::OutputDebugMessage("ERROR: This object is not a client");
		return false;
	}

	if (connect(mSocket, (sockaddr*)&mExternalAddr, sizeof(mExternalAddr)) < 0) {
		Util::OutputDebugMessage("ERROR: Fail to connect");
		return false;
	}

	mLinkedServer.socket = mSocket;
	mLinkedServer.address = mExternalAddr;

	Util::OutputDebugMessage("=== Connect: success ===");
	//mpThreadClient = std::thread(&TCPSocket::ClientLiveLoop, this);
	mClientLiveLoopWorker = new ClientLiveLoopWorker(this);
	mClientLiveLoopWorker->Init("TCPSocketClientLiveLoop");

	return true;
}


void TCPSocket::ClientLiveLoop()
{
	if (mSocketType != _SOCKET_PLUGIN_TYPE_CLIENT) {
		Util::OutputDebugMessage("ERROR: This object is not a client");
		return;
	}

	mFIsThreadAlive = true;

	fd_set readfd;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000;
	SOCKET maxfd;
	int activity = 0;
	BYTE *recvbuffer = new BYTE[_SOCKET_PLUGIN_MAX_BUFFER];


	while (!mFShutdown) {
		FD_ZERO(&readfd);
		FD_SET(mSocket, &readfd);
		maxfd = mSocket;

		activity = select(maxfd + 1, &readfd, NULL, NULL, &timeout);
		if ((activity < 0) && (errno != EINTR)) {
			Util::OutputDebugMessage("Select error");
		}

		if (FD_ISSET(mSocket, &readfd)) {		// server send something
			int valread = 0;
			if ((valread = recv(mSocket, (char *)recvbuffer, _SOCKET_PLUGIN_MAX_BUFFER, 0)) == -1) {
				ClientLoseLink();
			}
			else {
				std::lock_guard<std::mutex> lock(mMutexThreadClientRecv);
				ClientRecvPacket(recvbuffer, valread);
			}
		}
	}

	delete recvbuffer;
	mFShutdownCheck = true;
	Util::OutputDebugMessage("Client shutdown");
}

void TCPSocket::ClientLoseLink()
{
	std::ostringstream oss;
	oss << "Lose connection." << std::endl;
	Util::OutputDebugMessage(oss.str().c_str());

	Shutdown();
}

void TCPSocket::ClientRecvPacket(BYTE *Data, unsigned int BufferSize)
{
	mAryRecvData.insert(mAryRecvData.end(), Data, Data + BufferSize);

	std::string patternStr = mPacketPattern;
	int patternLen = patternStr.length();
	char* pattern = new char[patternLen];
	memcpy_s(pattern, patternLen, patternStr.c_str(), patternLen);

	UnpackMessage(&mLinkedServer, pattern, patternLen);

	delete pattern;
}

void TCPSocket::RecvDataCallback(DataPacket packet)
{

}

void TCPSocket::Send(std::string Message, std::vector<LinkInfo> Targets /*= std::vector<LinkInfo>()*/)
{
	char* packedMsg = nullptr;
	long long packedMshLen;
	PackMessage(Message, &packedMsg, packedMshLen);

	switch (mSocketType)
	{
	case _SOCKET_PLUGIN_TYPE_NONE:
		break;
	case _SOCKET_PLUGIN_TYPE_SERVER:
		SendToClient(packedMsg, packedMshLen, Targets);
		break;
	case _SOCKET_PLUGIN_TYPE_CLIENT:
		SendToServer(packedMsg, packedMshLen);
		break;
	default:
		break;
	}

	if (packedMsg != nullptr) {
		delete packedMsg;
	}
}

void TCPSocket::SetPacketPattern(std::string Pattern)
{
	mPacketPattern = Pattern;
}

void TCPSocket::SendToServer(char* Data, long long DataLen)
{
	SendImplement(mSocket, Data, DataLen, 0);
}

void TCPSocket::SendToClient(char* Data, long long DataLen, std::vector<LinkInfo> Targets)
{
	std::vector<LinkInfo>* pArySocketClients = &Targets;
	if (Targets.size() == 0) {
		pArySocketClients = &mLinkedClients;
	}
	for (int i = 0; i < pArySocketClients->size(); i++) {
		SOCKET client = pArySocketClients->at(i).socket;
		SendImplement(client, Data, DataLen, 0);
	}
}

void TCPSocket::SendImplement(SOCKET s, const char* buf, long long len, int flags)
{
	send(s, buf, (int)len, flags);
}

void TCPSocket::PackMessage(std::string Message, char** ppData, long long& Len)
{
	std::string patternStr = mPacketPattern;
	long long patternLen = patternStr.length();
	long long messageLen = Message.length();
	char messageLenInChar[8] = { 0 };
	memcpy_s(messageLenInChar, sizeof(messageLenInChar), &messageLen, sizeof(messageLen));

	Len = patternLen + 8 + messageLen;
	*ppData = new char[patternLen + 8 + messageLen];
	memcpy_s(
		*ppData,
		Len,
		patternStr.c_str(),
		patternLen);
	memcpy_s(
		*ppData + patternLen,
		Len - patternLen,
		messageLenInChar,
		8);
	memcpy_s(
		*ppData + patternLen + 8,
		Len - patternLen - 8,
		Message.c_str(),
		messageLen);
}

void TCPSocket::UnpackMessage(LinkInfo* pSourceInfo, char* Pattern, int PatternLen)
{
	int bufferIndex = mAryRecvData.size();
	std::vector<int> indices = DatacmpIndex((char*)&mAryRecvData[0], mAryRecvData.size(), Pattern, PatternLen);

	if (mMapProcessedPacket.find(pSourceInfo) == mMapProcessedPacket.end()) {
		mMapProcessedPacket.insert(std::pair<LinkInfo*, std::vector<DataPacket>>(pSourceInfo, std::vector<DataPacket>()));
	}

	std::vector<DataPacket> *pAryProcessedPacket = &mMapProcessedPacket[pSourceInfo];

	if (indices.size() > 0)
	{
		// data before index: append on previous array
		if (pAryProcessedPacket->size() > 0)
		{
			int subBufferLen = indices[0];
			char* subBuffer = new char[subBufferLen];
			memcpy_s(subBuffer, subBufferLen, &mAryRecvData[0], subBufferLen);
			DataPacket packet = pAryProcessedPacket->at(pAryProcessedPacket->size() - 1);
			packet.Data.insert(packet.Data.end(), subBuffer, subBuffer + subBufferLen);
			packet.RecvCounter += subBufferLen;

			delete subBuffer;
		}

		// data after index: create a new array
		long remainDataIndex = 0;
		for (int i = 0; i < indices.size(); i++)
		{
			if (indices[i] + PatternLen + 8 > bufferIndex)
			{
				remainDataIndex = indices[i];
				break;
			}

			// pick data length
			char* dataLenInChar = new char[8];
			memcpy_s(dataLenInChar, 8, &mAryRecvData[0] + indices[i] + PatternLen, 8);
			long long dataLen = 0;
			memcpy_s(&dataLen, 8, dataLenInChar, 8);

			delete dataLenInChar;

			// pick data
			long indexBegin = indices[i] + PatternLen + 8;
			long long indexExpect = indexBegin + dataLen;
			long indexEnd = i + 1 >= indices.size() ? (indexExpect > bufferIndex ? bufferIndex : indexExpect) : indices[i + 1];
			long subBufferLen = indexEnd - indexBegin;
			char* subBuffer = new char[indexEnd - indexBegin];
			memcpy_s(subBuffer, subBufferLen, &mAryRecvData[0] + indexBegin, subBufferLen);

			DataPacket packet;
			packet.Data.insert(packet.Data.end(), subBuffer, subBuffer + subBufferLen);
			packet.Len = dataLen;
			packet.RecvCounter += subBufferLen;
			memcpy_s(&packet.Owner, sizeof(LinkInfo), pSourceInfo, sizeof(LinkInfo));

			delete subBuffer;

			pAryProcessedPacket->push_back(packet);
			remainDataIndex = indexEnd;
		}

		long remainBufferLen = bufferIndex - remainDataIndex;
		char* remainBuffer = new char[remainBufferLen];
		memcpy_s(remainBuffer, remainBufferLen, &mAryRecvData[0] + remainDataIndex, remainBufferLen);
		mAryRecvData.clear();
		mAryRecvData.insert(mAryRecvData.end(), remainBuffer, remainBuffer + remainBufferLen);
		bufferIndex = remainBufferLen;

		delete remainBuffer;
	}

	// receive data callback
	for (int i = 0; i < pAryProcessedPacket->size(); i++) {
		DataPacket packet = pAryProcessedPacket->at(i);
		if (packet.RecvCounter == packet.Len)
		{
			std::future<void> asyncResult = std::async(std::launch::async, &TCPSocket::RecvDataCallback, this, packet);

			pAryProcessedPacket->erase(pAryProcessedPacket->begin() + i);
			i--;
		}
	}
}

std::vector<int> TCPSocket::DatacmpIndex(char* Data, int DataSize, char* Pattern, int PatternSize, int MaxCount /*= 0*/)
{
	std::vector<int> output;

	if (PatternSize == 0)
		return output;
	if (DataSize < PatternSize)
		return output;

	for (int i = 0; i < DataSize; i++)
	{
		bool isFound = false;
		if (Data[i] == Pattern[0])
		{
			isFound = true;
			for (int j = 0; j < PatternSize; j++)
			{
				if (i + j >= DataSize)
				{
					isFound = false;
					break;
				}
				if (Data[i + j] != Pattern[j])
				{
					isFound = false;
					break;
				}
			}
		}
		if (isFound)
		{
			output.push_back(i);
			if (MaxCount <= 0)
				continue;
			else
			{
				if (output.size() == MaxCount)
					break;
			}
		}
	}

	return output;
}

void TCPSocket::Shutdown()
{
	if (mFIsThreadAlive) {
		mFShutdown = true;

		if (mSocketType == _SOCKET_PLUGIN_TYPE_SERVER && mServerLiveLoopWorker != nullptr) {
			mServerLiveLoopWorker->Stop();
			delete mServerLiveLoopWorker;
			mServerLiveLoopWorker = nullptr;
		}
		if (mSocketType == _SOCKET_PLUGIN_TYPE_CLIENT && mClientLiveLoopWorker != nullptr) {
			mClientLiveLoopWorker->Stop();
			delete mClientLiveLoopWorker;
			mClientLiveLoopWorker = nullptr;
		}

		shutdown(mSocket, SD_SEND);
		closesocket(mSocket);
	}

	mFIsThreadAlive = false;
}

bool TCPSocket::IsAlive()
{
	return mFIsThreadAlive;
}

void TCPSocket::Clear()
{
	if (mServerLiveLoopWorker != nullptr) {
		mServerLiveLoopWorker->Stop();
		delete mServerLiveLoopWorker;
		mServerLiveLoopWorker = nullptr;
	}
	if (mClientLiveLoopWorker != nullptr) {
		mClientLiveLoopWorker->Stop();
		delete mClientLiveLoopWorker;
		mClientLiveLoopWorker = nullptr;
	}

	Shutdown();

	if (mSocket != 0) {
		closesocket(mSocket);
	}
	mSocket = 0;
	mSocketType = _SOCKET_PLUGIN_TYPE_NONE;
	memset(&mLocalAddr, 0, sizeof(sockaddr_in));
	memset(&mExternalAddr, 0, sizeof(sockaddr_in));
	memset(&mLinkedServer, 0, sizeof(LinkInfo));
	mLinkedClients.clear();
	mFIsThreadAlive = false;
	mFShutdown = false;
	mFShutdownCheck = false;
}
