// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LevelSequence.h"
#include "MovieSceneTrack.h"
#include "MovieSceneSection.h"
#include "Sections/MovieSceneEventTriggerSection.h"
#include "Channels/MovieSceneEvent.h"
#include "Channels/MovieSceneEventChannel.h"
#include "Containers/ArrayView.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LouiePluginFunctionLibrary.generated.h"



USTRUCT(BlueprintType)
struct FKeyFrameData {
	GENERATED_BODY()

		FKeyFrameData() {
		FunctionName = "";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FunctionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FFrameNumber FrameNumber;
};
USTRUCT(BlueprintType)
struct FSectionKeyFrameData {
	GENERATED_BODY()

		FSectionKeyFrameData() {
		SectionName = "";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SectionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKeyFrameData> KeyFrameData;
};
USTRUCT(BlueprintType)
struct FTrackKeyFrameData {
	GENERATED_BODY()

		FTrackKeyFrameData() {
		TrackName = "";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TrackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FSectionKeyFrameData> SectionKeyFrameData;


};
/**
 * 
 */
UCLASS()
class LOUIEPLUGIN_API ULouiePluginFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		static void GetFirstEventSectionKeyFrames(const ULevelSequence *TargetLevelSequence, FSectionKeyFrameData &SectionKeyFrameData);
	
};
