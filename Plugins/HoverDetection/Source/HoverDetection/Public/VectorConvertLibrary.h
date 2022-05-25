// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HoverDetectionHandler.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorConvertLibrary.generated.h"

// 0 : 左
// 1 : 中
// 2 : 右
// 3 : 上
// 4 : 後
// 5 : 下

/**
 * 
 */
UCLASS()
class HOVERDETECTION_API UVectorConvertLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void GetActorVertexLocation(AHoverDetectionHandler* HoverDetectionHandler, AActor* Actor, TArray<FVector>& Vertices);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void WorldLocationToScreenPosition(FVector WorldLocation, FVector2D ScreenSize, AActor* Camera, TMap<int, float> ScreenRotateYaw, FVector2D& ScreenPosition);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void ScreenPositionToWorldLocation(FVector2D ScreenPosition, FVector2D ScreenSize, float ProjectionLength ,AActor* Camera, TMap<int, float> ScreenRotateYaw, FVector& WorldLocation);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void WorldLocationToScreenLookAtRotator(FVector WorldLocation, AActor* Camera, FRotator& Rotator);
};
