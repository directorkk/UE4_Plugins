// Fill out your copyright notice in the Description page of Project Settings.


#include "LouiePluginFunctionLibrary.h"

void ULouiePluginFunctionLibrary::GetFirstEventSectionKeyFrames(const ULevelSequence *TargetLevelSequence, FSectionKeyFrameData &SectionKeyFrameData)
{
	if (TargetLevelSequence != nullptr) {
		/*FTrackKeyFrameData trackKeyFrameData;
		trackKeyFrameData.TrackName = "TOTAL";

		TArray<UMovieSceneSection*> sections = TargetLevelSequence->GetMovieScene()->GetAllSections();
		for (int i = 0; i < sections.Num(); i++) {
			UMovieSceneSection* section = sections[i];
			UMovieSceneEventTriggerSection* eventTriggerSection = (UMovieSceneEventTriggerSection*)section;
			if (eventTriggerSection != nullptr) {
				FSectionKeyFrameData sectionKeyFrameData;
				sectionKeyFrameData.SectionName = eventTriggerSection->GetName();

				TRange<FFrameNumber> withinRange;
				TArray<FFrameNumber> outKeyTimes;
				TArray<FKeyHandle> outKeyHandles;
				eventTriggerSection->EventChannel.GetKeys(withinRange, &outKeyTimes, &outKeyHandles);
				for (int j = 0; j < outKeyTimes.Num(); j++) {
					const FFrameNumber frameNumber = outKeyTimes[j];
					
					sectionKeyFrameData.KeyFrameNumber.Add(frameNumber);
				}
				trackKeyFrameData.SectionKeyFrameData.Add(sectionKeyFrameData.SectionName, sectionKeyFrameData);
			}
		}

 		TrackKeyFrameData.Add(trackKeyFrameData.TrackName, trackKeyFrameData);*/

		TArray<UMovieSceneSection*> sections = TargetLevelSequence->GetMovieScene()->GetAllSections();
		for (UMovieSceneSection* section : sections) {
			UMovieSceneEventTriggerSection* eventTriggerSection = Cast<UMovieSceneEventTriggerSection>(section);
			if (eventTriggerSection != nullptr) {
				SectionKeyFrameData.SectionName = eventTriggerSection->GetName();

				TMovieSceneChannelData<FMovieSceneEvent> channelData = eventTriggerSection->EventChannel.GetData();
				TArrayView<FMovieSceneEvent> eventData = channelData.GetValues();

				TRange<FFrameNumber> withinRange;
				TArray<FFrameNumber> outKeyTimes;
				TArray<FKeyHandle> outKeyHandles;
				channelData.GetKeys(withinRange, &outKeyTimes, &outKeyHandles);
				//eventTriggerSection->EventChannel.GetKeys(withinRange, &outKeyTimes, &outKeyHandles);
				for (int i = 0; i < outKeyTimes.Num(); i++) {
					const FFrameNumber frameNumber = outKeyTimes[i];
					FString functionName = eventData[i].FunctionName.ToString();

					FKeyFrameData keyFrameData;
					keyFrameData.FunctionName = functionName;
					keyFrameData.FrameNumber = frameNumber;

					SectionKeyFrameData.KeyFrameData.Add(keyFrameData);
				}

				break;
			}
		}
	}
}
