// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PlayScene.h"

#include "SubSceneHandler.generated.h"

UCLASS()
class PLAYSCENE_API ASubSceneHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASubSceneHandler();

	UFUNCTION(BlueprintCallable)
		void OpenWindow(FString ClientName, FVector2D ClientSize, FVector2D ClientPosition);
	UFUNCTION(BlueprintCallable)
		void CloseWindow(FString ClientName);
	UFUNCTION(BlueprintCallable)
		void CloseAllWindows();
	UFUNCTION(BlueprintCallable)
		void SetRenderTargetTexture(FString ClientName, UTextureRenderTarget2D* RenderTargetTexture);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
