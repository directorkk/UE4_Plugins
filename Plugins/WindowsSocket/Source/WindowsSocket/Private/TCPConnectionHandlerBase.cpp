// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPConnectionHandlerBase.h"
using namespace WindowsSocket;

// Sets default values
ATCPConnectionHandlerBase::ATCPConnectionHandlerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATCPConnectionHandlerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATCPConnectionHandlerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Shutdown();
}

void ATCPConnectionHandlerBase::StartupServer(int LocalPort)
{
	InitServer(LocalPort);
	StartServer();
}

void ATCPConnectionHandlerBase::StartupClient(FString TargetIP, int TargetPort)
{
	InitClient(Util::wstos(*TargetIP), TargetPort);
	StartClient();
}

void ATCPConnectionHandlerBase::SendString(FString Message)
{
	Send(Util::wstos(*Message));
}

void ATCPConnectionHandlerBase::RecvDataCallback(DataPacket Packet)
{
	Packet.Data.push_back('\0');
	std::string msg = Packet.Data.data();

	Util::OutputDebugMessage(msg);
}

// Called every frame
void ATCPConnectionHandlerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

