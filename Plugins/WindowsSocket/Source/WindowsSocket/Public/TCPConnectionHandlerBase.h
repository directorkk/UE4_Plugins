// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TCPSocket.h"

#include "GameFramework/Actor.h"
#include "TCPConnectionHandlerBase.generated.h"

UCLASS()
class WINDOWSSOCKET_API ATCPConnectionHandlerBase : public AActor, public TCPSocket
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATCPConnectionHandlerBase();

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
