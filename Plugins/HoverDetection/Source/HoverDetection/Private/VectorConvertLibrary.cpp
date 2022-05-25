// Fill out your copyright notice in the Description page of Project Settings.


#include "VectorConvertLibrary.h"

// 0 : 左
// 1 : 中
// 2 : 右
// 3 : 上
// 4 : 後
// 5 : 下

void UVectorConvertLibrary::GetActorVertexLocation(AHoverDetectionHandler* HoverDetectionHandler, AActor* Actor, TArray<FVector>& Vertices)
{
	FVector actorLocation;
	FVector actorBound;
	HoverDetectionHandler->GetActorBound(Actor, actorLocation, actorBound);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				float x = (i == 0) ? -1 : 1;
				float y = (j == 0) ? -1 : 1;
				float z = (k == 0) ? -1 : 1;
				FRotator actorRotation = Actor->GetActorRotation();
				FVector vertexLocation = actorRotation.RotateVector(*new FVector(actorBound.X * x, actorBound.Y * y, actorBound.Z * z)) + actorLocation;
				Vertices.Add(vertexLocation);
			}
		}
	}
}

void UVectorConvertLibrary::WorldLocationToScreenPosition(FVector WorldLocation, FVector2D ScreenSize, AActor* Camera, TMap<int, float> ScreenRotateYaw, FVector2D& ScreenPosition)
{
	FVector2D uv;
	//Relative with Camera
	FVector relativeLocation = Camera->GetActorRotation().UnrotateVector(WorldLocation - Camera->GetActorLocation());
	FVector relativeLocationNormalize;
	//Width / Height
	float screenRatio = ScreenSize.X / ScreenSize.Y;
	float frontDistance = 0;
	float relativeLocation_abs_x = FMath::Abs(relativeLocation.X);
	float relativeLocation_abs_y = FMath::Abs(relativeLocation.Y);
	float relativeLocation_abs_z = FMath::Abs(relativeLocation.Z);
	int screenNumber = 0;

	if (relativeLocation_abs_x >= relativeLocation_abs_y && relativeLocation_abs_x >= relativeLocation_abs_z)
	{
		frontDistance = relativeLocation.X;
		//X axis 1:Fornt 4:Back
		screenNumber = frontDistance >= 0 ? 1 : 4;
	}
	else if(relativeLocation_abs_y >= relativeLocation_abs_x && relativeLocation_abs_y >= relativeLocation_abs_z)
	{
		frontDistance = relativeLocation.Y;
		//Y axis 0:Left 2:Right
		screenNumber = frontDistance <= 0 ? 0 : 2;
	}
	else if(relativeLocation_abs_z >= relativeLocation_abs_x && relativeLocation_abs_z >= relativeLocation_abs_y)
	{
		frontDistance = relativeLocation.Z;
		//Z axis 3:Top 5:Bottom
		screenNumber = frontDistance >= 0 ? 3 : 5;
	}
	float screenRotateYaw = ScreenRotateYaw.Contains(screenNumber) ? ScreenRotateYaw[screenNumber] : 0;
	relativeLocation = relativeLocation.RotateAngleAxis(screenRotateYaw, Camera->GetActorUpVector());
	relativeLocationNormalize = relativeLocation / FMath::Abs(frontDistance);
	switch (screenNumber)
	{
	case 0:
		uv = *new FVector2D(relativeLocationNormalize.X, relativeLocationNormalize.Z * screenRatio * -1);
		break;
	case 1:
		uv = *new FVector2D(relativeLocationNormalize.Y, relativeLocationNormalize.Z * screenRatio * -1);
		break;
	case 2:
		uv = *new FVector2D(relativeLocationNormalize.X * -1, relativeLocationNormalize.Z * screenRatio * -1);
		break;
	case 3:
		uv = *new FVector2D(relativeLocationNormalize.Y / screenRatio, relativeLocationNormalize.X / screenRatio);
		break;
	case 4:
		uv = *new FVector2D(relativeLocationNormalize.Y * -1, relativeLocationNormalize.Z * screenRatio * -1);
		break;
	case 5:
		uv = *new FVector2D(relativeLocationNormalize.Y / screenRatio, relativeLocationNormalize.X / screenRatio * -1);
		break;
	}
	uv = (uv / 2) + 0.5;
	ScreenPosition.X = ScreenSize.X * (screenNumber + uv.X);
	ScreenPosition.Y = ScreenSize.Y * uv.Y;
}

void UVectorConvertLibrary::ScreenPositionToWorldLocation(FVector2D ScreenPosition, FVector2D ScreenSize, float ProjectionLength, AActor* Camera, TMap<int, float> ScreenRotateYaw, FVector& WorldLocation)
{
	FVector relativeLocation = *new FVector();
	FVector relativeLocationNormalize = *new FVector();
	FVector2D uv = *new FVector2D();
	float screenRatio = ScreenSize.X / ScreenSize.Y;
	int screenNumber = floor(ScreenPosition.X / ScreenSize.X);
	uv.X = ((int)ScreenPosition.X % (int)ScreenSize.X) / ScreenSize.X - 0.5;
	uv.Y = ((int)ScreenPosition.Y % (int)ScreenSize.Y) / ScreenSize.Y - 0.5;
	uv *= 2;
	switch (screenNumber)
	{
	case 0:
		relativeLocationNormalize = *new FVector(uv.X, -1, uv.Y / screenRatio * -1);
		break;
	case 1:
		relativeLocationNormalize = *new FVector(1, uv.X, uv.Y / screenRatio * -1);
		break;
	case 2:
		relativeLocationNormalize = *new FVector(uv.X * -1, 1, uv.Y / screenRatio * -1);
		break;
	case 3:
		relativeLocationNormalize = *new FVector(uv.Y * screenRatio, uv.X * screenRatio, 1);
		break;
	case 4:
		relativeLocationNormalize = *new FVector(uv.X * -1, -1, uv.Y / screenRatio * -1);
		break;
	case 5:
		relativeLocationNormalize = *new FVector(uv.Y * screenRatio * -1, uv.X * screenRatio, 1);
		break;
	}
	float screenRotateYaw = ScreenRotateYaw.Contains(screenNumber) ? ScreenRotateYaw[screenNumber] : 0;
	relativeLocation = relativeLocationNormalize * ProjectionLength;
	relativeLocation = relativeLocation.RotateAngleAxis(screenRotateYaw, Camera->GetActorUpVector());
	WorldLocation = Camera->GetActorLocation() + Camera->GetActorRotation().RotateVector(relativeLocation);
}

void UVectorConvertLibrary::WorldLocationToScreenLookAtRotator(FVector WorldLocation, AActor * Camera, FRotator& Rotator)
{
	FRotator lookAtRotator = FRotationMatrix::MakeFromX(WorldLocation - Camera->GetActorLocation()).Rotator();
	Rotator = lookAtRotator;
}
