// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <k4a/k4a.hpp>
#include <k4abt.hpp>

#include "Util.h"
#include "Vector3d.h"

#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>

#include "CoreMinimal.h"

/**
 *
 */
struct ProcessedBoneData;
struct ProcessedBodyData;
struct ProcessedFrameData;

class AzureKinectWrapper {
public:
	AzureKinectWrapper();
	~AzureKinectWrapper();

	void SetupCaptureSetting(bool IsCaptureDepthFrame, bool IsCaptureColorFrame);
	void StartLiveLoop();
	void LiveLoop();
	bool IsAlive();
	void Shutdown();

	ProcessedFrameData PopData();

	std::string GetJointNameById(k4abt_joint_id_t Id);

private:
	std::thread mThreadLiveLoop;
	std::mutex mMutexProcessData;
	k4a::device mDevice;
	k4abt::tracker mTracker;
	k4a::calibration mCalibration;
	k4a::transformation mTransformation;
	bool mIsCaptureDepthFrame;
	bool mIsCaptureColorFrame;

	std::array<std::pair<k4abt_joint_id_t, k4abt_joint_id_t>, 31> mBoneList;
	std::unordered_map<k4abt_joint_id_t, std::string> mJointNames;
	void InitJointBoneInformation();

	bool mFIsThreadAlive;
	bool mFShutdown;

	std::vector<ProcessedFrameData> mAryProcessedFrameData;
	int mProcessedFrameDataLimit;
	void ProcessData(k4a::image ColorImage, k4a::image DepthImage, k4abt::frame Frame);
};

struct ProcessedBoneData {
	int RootJointId;
	Vector3 Vertices[2];
	Vector4 Quaternion;
	bool IsConfidentBone;
};
struct ProcessedBodyData {
	int ConfidenceValue;
	std::map<int, ProcessedBoneData, std::greater<int>> Bones;
};
struct ProcessedFrameData {
	std::vector<ProcessedBodyData> Bodies;

	int Width;
	int Height;
	int Channel;
	// need to be release manually
	unsigned char *Data;
	unsigned char *ColorData;

	ProcessedFrameData() {
		Width = Height = Channel = 0;
		Data = nullptr;
		ColorData = nullptr;
	}
};
