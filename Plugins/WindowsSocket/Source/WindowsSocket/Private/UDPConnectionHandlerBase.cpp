// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPConnectionHandlerBase.h"
using namespace WindowsSocket;

// Sets default values
AUDPConnectionHandlerBase::AUDPConnectionHandlerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUDPConnectionHandlerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUDPConnectionHandlerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Shutdown();
}

void AUDPConnectionHandlerBase::StartupServer(int LocalPort)
{
	InitServer(LocalPort);
	StartServer();
}

void AUDPConnectionHandlerBase::StartupClient(FString TargetIP, int TargetPort)
{
	InitClient(Util::wstos(*TargetIP), TargetPort);
	StartClient();
}

void AUDPConnectionHandlerBase::SendString(FString Message)
{
	Send(Util::wstos(*Message));
}

void AUDPConnectionHandlerBase::RecvDataCallback(DataPacket Packet)
{
	Packet.Data.push_back('\0');
	std::string msg = Packet.Data.data();

	Util::OutputDebugMessage(msg);
}

// Called every frame
void AUDPConnectionHandlerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

