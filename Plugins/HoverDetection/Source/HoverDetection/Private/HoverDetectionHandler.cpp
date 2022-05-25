// Fill out your copyright notice in the Description page of Project Settings.


#include "HoverDetectionHandler.h"
#include "VectorConvertLibrary.h"

// Sets default values
AHoverDetectionHandler::AHoverDetectionHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHoverDetectionHandler::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHoverDetectionHandler::Tick(float DeltaTime)
{
	mDeltatime = DeltaTime;
	Super::Tick(DeltaTime);
}

void AHoverDetectionHandler::FindActorsInRange(TArray<AActor*> Actors, FVector2D UserScreenPosition, FVector2D UserLockRegion, FVector2D ScreenSize, AActor* Camera, TArray<AActor*>& ActorsInRange)
{
	TArray<AActor*> actors = Actors;
	for (int i = 0; i < Actors.Num(); i++)
	{
		AActor* actor = Actors[i];
		if (actor)
		{
			bool filter = false;
			FilterActor(actor, filter);
			if (filter)
			{
				actors.Remove(actor);
				continue;
			}
		}
		else
		{
			actors.Remove(actor);
			continue;
		}		
	}
	FVector userWorldLocation;
	UVectorConvertLibrary::ScreenPositionToWorldLocation(UserScreenPosition, ScreenSize, 1, Camera, mScreenRotateYaw, userWorldLocation);
	FRotator userScreenLookAtRotator;
	UVectorConvertLibrary::WorldLocationToScreenLookAtRotator(userWorldLocation, Camera, userScreenLookAtRotator);
	float userYawMin   = userScreenLookAtRotator.Yaw - UserLockRegion.X;
	float userYawMax   = userScreenLookAtRotator.Yaw + UserLockRegion.X;
	float userPitchMin = userScreenLookAtRotator.Pitch - UserLockRegion.Y;
	float userPitchMax = userScreenLookAtRotator.Pitch + UserLockRegion.Y;
	for (int i = 0; i < actors.Num(); i++)
	{
		AActor* actor = actors[i];
		float vertexYawMin = 0;
		float vertexYawMax = 0;
		float vertexPitchMin = 0;
		float vertexPitchMax = 0;
		TArray<FVector> vertices;
		UVectorConvertLibrary::GetActorVertexLocation(this, actor, vertices);
		for (int j = 0; j < vertices.Num(); j++)
		{
			FVector vertex = vertices[j];
			FRotator vertexScreenLookAtRotator;
			UVectorConvertLibrary::WorldLocationToScreenLookAtRotator(vertex, Camera, vertexScreenLookAtRotator);
			if (j == 0)
			{
				vertexYawMin   = vertexScreenLookAtRotator.Yaw;
				vertexYawMax   = vertexScreenLookAtRotator.Yaw;
				vertexPitchMin = vertexScreenLookAtRotator.Pitch;
				vertexPitchMax = vertexScreenLookAtRotator.Pitch;
			}
			else
			{
				vertexYawMin   = vertexScreenLookAtRotator.Yaw < vertexYawMin	  ? vertexScreenLookAtRotator.Yaw	: vertexYawMin;
				vertexYawMax   = vertexScreenLookAtRotator.Yaw > vertexYawMax	  ? vertexScreenLookAtRotator.Yaw	: vertexYawMax;
				vertexPitchMin = vertexScreenLookAtRotator.Pitch < vertexPitchMin ? vertexScreenLookAtRotator.Pitch : vertexPitchMin;
				vertexPitchMax = vertexScreenLookAtRotator.Pitch > vertexPitchMax ? vertexScreenLookAtRotator.Pitch : vertexPitchMax;
			}
		}
		if ((userYawMax > vertexYawMin && vertexYawMax > userYawMin) && (userPitchMax > vertexPitchMin && vertexPitchMax > userPitchMin))
		{
			ActorsInRange.Add(actor);
		}
	}
}

void AHoverDetectionHandler::CalculateLock(int aIndex, TArray<AActor*> aActorsInRange, FLockActor aLockActor, FLockActor& oLockActor)
{
	aLockActor.ActorsInRange = aActorsInRange;
	for (int a = 0; a < aLockActor.ActorsInRange.Num(); a++)
	{
		if (!aLockActor.ActorsInRange[a])
		{
			aLockActor.ActorsInRange.Remove(aLockActor.ActorsInRange[a]);
			continue;
		}
		if (
			!aLockActor.ActorsWatingForLock.Find(aLockActor.ActorsInRange[a]) &&
			aLockActor.ActorsLocked.Find(aLockActor.ActorsInRange[a]) == -1 // index will be -1 when it's not a valid index (auto in c++)
			)
		{
			StartLockActor(aLockActor.ActorsInRange[a], aIndex);
			aLockActor.ActorsWatingForLock.Add(aLockActor.ActorsInRange[a], 0);//start calculating it's locktime
		}
	}
	TArray<AActor*> keysForLock;
	aLockActor.ActorsWatingForLock.GetKeys(keysForLock);
	for (int i = 0; i < keysForLock.Num(); i++) {
		AActor* key = keysForLock[i];
		float val = aLockActor.ActorsWatingForLock[key];
		if (!key)
		{
			keysForLock.Remove(key);
			aLockActor.ActorsWatingForLock.Remove(key);
			continue;
		}
		if (aLockActor.ActorsInRange.Find(key) != -1)
		{
			if ((val + mDeltatime) < aLockActor.LockTime)
			{
				aLockActor.ActorsWatingForLock.Add(key, val + mDeltatime);
				LockingActor(key, aIndex, val + mDeltatime, aLockActor.LockTime);
			}
			else
			{
				FinishLockActor(key, aIndex);
				aLockActor.ActorsWatingForLock.Remove(key);
				aLockActor.ActorsLocked.Add(key);//Lock this actor
			}
		}
		else
		{
			FinishUnlockActor(key, aIndex);
			aLockActor.ActorsWatingForLock.Remove(key);
		}
	}
	for (int a = 0; a < aLockActor.ActorsLocked.Num(); a++)
	{
		if (!aLockActor.ActorsLocked[a])
		{
			aLockActor.ActorsLocked.Remove(aLockActor.ActorsLocked[a]);
			continue;
		}
		if (
			!aLockActor.ActorsWatingForUnlock.Find(aLockActor.ActorsLocked[a]) &&
			aLockActor.ActorsInRange.Find(aLockActor.ActorsLocked[a]) == -1
			)
		{
			StartUnlockActor(aLockActor.ActorsLocked[a], aIndex);
			aLockActor.ActorsWatingForUnlock.Add(aLockActor.ActorsLocked[a], 0);//start calculating it's unlocktime
		}
	}
	TArray<AActor*> keysForUnlock;
	aLockActor.ActorsWatingForUnlock.GetKeys(keysForUnlock);
	for (int i = 0; i < keysForUnlock.Num(); i++)
	{
		AActor* key = keysForUnlock[i];
		float val = aLockActor.ActorsWatingForUnlock[key];
		if (!key)
		{
			keysForUnlock.Remove(key);
			aLockActor.ActorsWatingForUnlock.Remove(key);
			continue;
		}
		if (aLockActor.ActorsInRange.Find(key) == -1)
		{
			if ((val + mDeltatime) < aLockActor.UnlockTime)
			{
				aLockActor.ActorsWatingForUnlock.Add(key, val + mDeltatime);
				UnlockingActor(key, aIndex, val + mDeltatime, aLockActor.UnlockTime);
			}
			else
			{
				FinishUnlockActor(key, aIndex);
				aLockActor.ActorsWatingForUnlock.Remove(key);
				aLockActor.ActorsLocked.Remove(key);//Unlock this actor
			}
		}
		else
		{
			aLockActor.ActorsWatingForUnlock.Remove(key);
		}
	}
	oLockActor = aLockActor;
}

void AHoverDetectionHandler::GetActorBound(AActor* Actor, FVector& ActorLocation, FVector& ActorBound)
{
	UClass* actorClass = Actor->GetClass();
	FVector actorLocation = Actor->GetActorLocation();
	FVector actorBound;

	if (mActorBound.Contains(actorClass))
	{
		actorBound = mActorBound[actorClass];
	}
	else
	{
		AActor* actor = GetWorld()->SpawnActor(actorClass, new FTransform());
		actorBound = actor->GetComponentsBoundingBox().GetExtent();
		mActorBound.Add(actorClass,actorBound);
		GetWorld()->DestroyActor(actor);
	}

	ActorLocation = actorLocation;
	ActorBound = actorBound * Actor->GetActorScale3D();
}

