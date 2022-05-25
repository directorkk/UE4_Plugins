// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UDPSocket.h"

#include "GameFramework/Actor.h"
#include "UDPConnectionHandlerBase.generated.h"


struct ScheduledPacket {
	bool State;
	DataPacket Packet;

	ScheduledPacket() {
		State = false;
	}
};


UCLASS()
class WINDOWSSOCKET_API AUDPConnectionHandlerBase : public AActor, public UDPSocket
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUDPConnectionHandlerBase();

	UFUNCTION(BlueprintCallable)
		void StartupServer(int LocalPort);
	UFUNCTION(BlueprintCallable)
		void StartupClient(FString TargetIP, int TargetPort);
	UFUNCTION(BlueprintCallable)
		void SendString(FString Message);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void RecvDataCallback(DataPacket Packet) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
