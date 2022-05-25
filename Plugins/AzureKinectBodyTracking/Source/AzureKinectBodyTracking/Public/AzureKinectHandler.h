// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AzureKinectWrapper.h"
#include <fstream>

#include "GameFramework/Actor.h"
#include "AzureKinectHandler.generated.h"

USTRUCT(BlueprintType)
struct FKinectBoneData {
	GENERATED_BODY()

		FKinectBoneData() {

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RootJointId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> Vertices;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FQuat Quaternion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsConfidentBone;

	void FromWrapper(ProcessedBoneData BoneData) {
		RootJointId = BoneData.RootJointId;
		IsConfidentBone = BoneData.IsConfidentBone;
		Vertices.Empty();
		Vertices.Add(FVector(BoneData.Vertices[0].x, BoneData.Vertices[0].y, BoneData.Vertices[0].z));
		Vertices.Add(FVector(BoneData.Vertices[1].x, BoneData.Vertices[1].y, BoneData.Vertices[1].z));
		Quaternion = FQuat(BoneData.Quaternion.x, BoneData.Quaternion.y, BoneData.Quaternion.z, BoneData.Quaternion.w);
	}
};

USTRUCT(BlueprintType)
struct FKinectBodyData {
	GENERATED_BODY()

		FKinectBodyData() {

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ConfidenceValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int, FKinectBoneData> Bones;

	void FromWrapper(ProcessedBodyData BodyData) {
		ConfidenceValue = BodyData.ConfidenceValue;
		Bones.Empty();
		for (std::map<int, ProcessedBoneData>::iterator it = BodyData.Bones.begin(); it != BodyData.Bones.end(); it++) {
			ProcessedBoneData boneData = it->second;
			FKinectBoneData kinectBoneData;
			kinectBoneData.FromWrapper(boneData);
			Bones.Add(it->first, kinectBoneData);
		}
	}
};

USTRUCT(BlueprintType)
struct FKinectFrameData {
	GENERATED_BODY()

		FKinectFrameData() {
		Width = Height = Channel = 0;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKinectBodyData> Bodies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Channel;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	UTexture2D *Data;

	void FromWrapper(ProcessedFrameData FrameData) {
		Width = FrameData.Width;
		Height = FrameData.Height;
		Channel = FrameData.Channel;
		Bodies.Empty();
		for (int i = 0; i < FrameData.Bodies.size(); i++) {
			ProcessedBodyData bodyData = FrameData.Bodies.at(i);
			FKinectBodyData kinectBodyData;
			kinectBodyData.FromWrapper(bodyData);
			Bodies.Add(kinectBodyData);
		}
	}
};


UCLASS()
class AZUREKINECTBODYTRACKING_API AAzureKinectHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAzureKinectHandler();

	UFUNCTION(BlueprintCallable)
		void Start();
	UFUNCTION(BlueprintCallable)
		void SetupCaptureSetting(bool IsCaptureDepthFrame, bool IsCaptureColorFrame);
	UFUNCTION(BlueprintCallable)
		void PopFrameData(bool IsDepthTextureUpdate, UTexture2D *DepthTexture, bool IsColorTextureUpdate, UTexture2D *ColorTexture, bool &Result, FKinectFrameData &FrameData);
	UFUNCTION(BlueprintCallable)
		void Shutdown();
	UFUNCTION(BlueprintCallable)
		void CreateTexture2DTransient(int Width, int Height, int Amount, TArray<UTexture2D*> &OutputTextures);

private:
	AzureKinectWrapper *mpAzureKinectWrapper;
	bool mIsCaptureDepthFrame;
	bool mIsCaptureColorFrame;

	void UpdateDepthTexture(UTexture2D *DepthTexture, ProcessedFrameData FrameData);
	void UpdateColorTexture(UTexture2D *ColorTexture, ProcessedFrameData FrameData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
