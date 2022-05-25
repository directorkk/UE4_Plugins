// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSceneHandler.h"

// Sets default values
ASubSceneHandler::ASubSceneHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASubSceneHandler::OpenWindow(FString ClientName, FVector2D ClientSize, FVector2D ClientPosition)
{
	FPlaySceneModule* pPlaySceneModule = (FPlaySceneModule*)&FPlaySceneModule::Get();
	pPlaySceneModule->OpenWindow(ClientName, ClientSize, ClientPosition);

}

void ASubSceneHandler::CloseWindow(FString ClientName)
{
	FPlaySceneModule* pPlaySceneModule = (FPlaySceneModule*)&FPlaySceneModule::Get();
	pPlaySceneModule->CloseWindow(ClientName);
}

void ASubSceneHandler::CloseAllWindows()
{
	FPlaySceneModule* pPlaySceneModule = (FPlaySceneModule*)&FPlaySceneModule::Get();
	pPlaySceneModule->CloseAllWindows();
}

void ASubSceneHandler::SetRenderTargetTexture(FString ClientName, UTextureRenderTarget2D* RenderTargetTexture)
{
	IPlayScene::Get().SetTextureRenderTarget2D(ClientName, RenderTargetTexture);
}


// Called when the game starts or when spawned
void ASubSceneHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASubSceneHandler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FPlaySceneModule* pPlaySceneModule = (FPlaySceneModule*)&FPlaySceneModule::Get();
	pPlaySceneModule->CloseAllWindows();
}

// Called every frame
void ASubSceneHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

