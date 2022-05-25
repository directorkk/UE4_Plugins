// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#include "Engine/World.h"

#include "HoverDetectionHandler.generated.h"


USTRUCT(BlueprintType)
struct FLockActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		float				 LockTime;
	UPROPERTY(BlueprintReadWrite)
		float				 UnlockTime;
	UPROPERTY(BlueprintReadWrite)
		TArray<AActor*>		 ActorsInRange;
	UPROPERTY(BlueprintReadWrite)
		TArray<AActor*>		 ActorsLocked;
	UPROPERTY(BlueprintReadWrite)
		TMap<AActor*, float> ActorsWatingForLock;
	UPROPERTY(BlueprintReadWrite)
		TMap<AActor*, float> ActorsWatingForUnlock;
	FLockActor() {
		LockTime = 1;
		UnlockTime = 1;
		ActorsInRange.Empty();
		ActorsLocked.Empty();
		ActorsWatingForLock.Empty();
		ActorsWatingForUnlock.Empty();
	}
};

UCLASS()
class HOVERDETECTION_API AHoverDetectionHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHoverDetectionHandler();

	UPROPERTY(BlueprintReadWrite)
	TMap<int, float> mScreenRotateYaw;

private:
	float mDeltatime;
	TMap<UClass*, FVector> mActorBound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void FindActorsInRange(TArray<AActor*> Actors, FVector2D UserScreenPosition, FVector2D UserLockRegion, FVector2D ScreenSize, AActor* Camera, TArray<AActor*>& ActorsInRange);
	UFUNCTION(BlueprintCallable)
		void CalculateLock(int aIndex, TArray<AActor*> aActorsInRange, FLockActor aLockActor, FLockActor& oLockActor);
	UFUNCTION(BlueprintCallable)
		void GetActorBound(AActor* Actor, FVector& ActorLocation, FVector& ActorBound);

	UFUNCTION(BlueprintImplementableEvent)
		void StartLockActor(const AActor* Actor, int Index);
	UFUNCTION(BlueprintImplementableEvent)
		void FinishLockActor(const AActor* Actor, int Index);
	UFUNCTION(BlueprintImplementableEvent)
		void StartUnlockActor(const AActor* Actor, int Index);
	UFUNCTION(BlueprintImplementableEvent)
		void FinishUnlockActor(const AActor* Actor, int Index);
	UFUNCTION(BlueprintImplementableEvent)
		void LockingActor(const AActor* Actor, int Index, float CurrentLockingTime, float LockingTime);
	UFUNCTION(BlueprintImplementableEvent)
		void UnlockingActor(const AActor* Actor, int Index, float CurrentUnlockingTime, float UnlockingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Override")
		void FilterActor(const AActor* Actor, bool& IsFilter);
};
