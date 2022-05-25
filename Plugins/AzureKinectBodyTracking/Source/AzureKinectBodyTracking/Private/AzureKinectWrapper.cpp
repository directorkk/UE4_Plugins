#include "AzureKinectWrapper.h"
using namespace AzureKinectBodyTracking;

AzureKinectWrapper::AzureKinectWrapper()
{
	mFIsThreadAlive = false;
	mFShutdown = false;
	mIsCaptureDepthFrame = false;
	mIsCaptureColorFrame = false;
	mProcessedFrameDataLimit = 100;

	InitJointBoneInformation();
}

AzureKinectWrapper::~AzureKinectWrapper()
{

}

void AzureKinectWrapper::SetupCaptureSetting(bool IsCaptureDepthFrame, bool IsCaptureColorFrame)
{
	mIsCaptureDepthFrame = IsCaptureDepthFrame;
	mIsCaptureColorFrame = IsCaptureColorFrame;
}

void AzureKinectWrapper::InitJointBoneInformation()
{
	// Define the bone list based on the documentation
	// every pair is a single bone
	mBoneList = {
	std::make_pair(K4ABT_JOINT_SPINE_CHEST, K4ABT_JOINT_SPINE_NAVAL),
	std::make_pair(K4ABT_JOINT_PELVIS, K4ABT_JOINT_SPINE_NAVAL),
	std::make_pair(K4ABT_JOINT_NECK, K4ABT_JOINT_SPINE_CHEST),
	std::make_pair(K4ABT_JOINT_HEAD, K4ABT_JOINT_NECK),
	std::make_pair(K4ABT_JOINT_NOSE, K4ABT_JOINT_HEAD),

	std::make_pair(K4ABT_JOINT_CLAVICLE_LEFT, K4ABT_JOINT_SPINE_CHEST),
	std::make_pair(K4ABT_JOINT_SHOULDER_LEFT, K4ABT_JOINT_CLAVICLE_LEFT),
	std::make_pair(K4ABT_JOINT_ELBOW_LEFT, K4ABT_JOINT_SHOULDER_LEFT),
	std::make_pair(K4ABT_JOINT_WRIST_LEFT, K4ABT_JOINT_ELBOW_LEFT),
	std::make_pair(K4ABT_JOINT_HAND_LEFT, K4ABT_JOINT_WRIST_LEFT),
	std::make_pair(K4ABT_JOINT_HANDTIP_LEFT, K4ABT_JOINT_HAND_LEFT),
	std::make_pair(K4ABT_JOINT_THUMB_LEFT, K4ABT_JOINT_WRIST_LEFT),
	std::make_pair(K4ABT_JOINT_HIP_LEFT, K4ABT_JOINT_PELVIS),
	std::make_pair(K4ABT_JOINT_KNEE_LEFT, K4ABT_JOINT_HIP_LEFT),
	std::make_pair(K4ABT_JOINT_ANKLE_LEFT, K4ABT_JOINT_KNEE_LEFT),
	std::make_pair(K4ABT_JOINT_FOOT_LEFT, K4ABT_JOINT_ANKLE_LEFT),
	std::make_pair(K4ABT_JOINT_EYE_LEFT, K4ABT_JOINT_NOSE),
	std::make_pair(K4ABT_JOINT_EAR_LEFT, K4ABT_JOINT_EYE_LEFT),

	std::make_pair(K4ABT_JOINT_CLAVICLE_RIGHT, K4ABT_JOINT_SPINE_CHEST),
	std::make_pair(K4ABT_JOINT_SHOULDER_RIGHT, K4ABT_JOINT_CLAVICLE_RIGHT),
	std::make_pair(K4ABT_JOINT_ELBOW_RIGHT, K4ABT_JOINT_SHOULDER_RIGHT),
	std::make_pair(K4ABT_JOINT_WRIST_RIGHT, K4ABT_JOINT_ELBOW_RIGHT),
	std::make_pair(K4ABT_JOINT_HAND_RIGHT, K4ABT_JOINT_WRIST_RIGHT),
	std::make_pair(K4ABT_JOINT_HANDTIP_RIGHT, K4ABT_JOINT_HAND_RIGHT),
	std::make_pair(K4ABT_JOINT_THUMB_RIGHT, K4ABT_JOINT_WRIST_RIGHT),
	std::make_pair(K4ABT_JOINT_HIP_RIGHT, K4ABT_JOINT_PELVIS),
	std::make_pair(K4ABT_JOINT_KNEE_RIGHT, K4ABT_JOINT_HIP_RIGHT),
	std::make_pair(K4ABT_JOINT_ANKLE_RIGHT, K4ABT_JOINT_KNEE_RIGHT),
	std::make_pair(K4ABT_JOINT_FOOT_RIGHT, K4ABT_JOINT_ANKLE_RIGHT),
	std::make_pair(K4ABT_JOINT_EYE_RIGHT, K4ABT_JOINT_NOSE),
	std::make_pair(K4ABT_JOINT_EAR_RIGHT, K4ABT_JOINT_EYE_RIGHT)
	};
	// Define the joint string names
	mJointNames = {
	std::make_pair(K4ABT_JOINT_PELVIS,        "PELVIS"),
	std::make_pair(K4ABT_JOINT_SPINE_NAVAL,   "SPINE_NAVAL"),
	std::make_pair(K4ABT_JOINT_SPINE_CHEST,   "SPINE_CHEST"),
	std::make_pair(K4ABT_JOINT_NECK,          "NECK"),
	std::make_pair(K4ABT_JOINT_CLAVICLE_LEFT, "CLAVICLE_LEFT"),
	std::make_pair(K4ABT_JOINT_SHOULDER_LEFT, "SHOULDER_LEFT"),
	std::make_pair(K4ABT_JOINT_ELBOW_LEFT,    "ELBOW_LEFT"),
	std::make_pair(K4ABT_JOINT_WRIST_LEFT,    "WRIST_LEFT"),
	std::make_pair(K4ABT_JOINT_HAND_LEFT,     "HAND_LEFT"),
	std::make_pair(K4ABT_JOINT_HANDTIP_LEFT,  "HANDTIP_LEFT"),
	std::make_pair(K4ABT_JOINT_THUMB_LEFT,    "THUMB_LEFT"),
	std::make_pair(K4ABT_JOINT_CLAVICLE_RIGHT,"CLAVICLE_RIGHT"),
	std::make_pair(K4ABT_JOINT_SHOULDER_RIGHT,"SHOULDER_RIGHT"),
	std::make_pair(K4ABT_JOINT_ELBOW_RIGHT,   "ELBOW_RIGHT"),
	std::make_pair(K4ABT_JOINT_WRIST_RIGHT,   "WRIST_RIGHT"),
	std::make_pair(K4ABT_JOINT_HAND_RIGHT,    "HAND_RIGHT"),
	std::make_pair(K4ABT_JOINT_HANDTIP_RIGHT, "HANDTIP_RIGHT"),
	std::make_pair(K4ABT_JOINT_THUMB_RIGHT,   "THUMB_RIGHT"),
	std::make_pair(K4ABT_JOINT_HIP_LEFT,      "HIP_LEFT"),
	std::make_pair(K4ABT_JOINT_KNEE_LEFT,     "KNEE_LEFT"),
	std::make_pair(K4ABT_JOINT_ANKLE_LEFT,    "ANKLE_LEFT"),
	std::make_pair(K4ABT_JOINT_FOOT_LEFT,     "FOOT_LEFT"),
	std::make_pair(K4ABT_JOINT_HIP_RIGHT,     "HIP_RIGHT"),
	std::make_pair(K4ABT_JOINT_KNEE_RIGHT,    "KNEE_RIGHT"),
	std::make_pair(K4ABT_JOINT_ANKLE_RIGHT,   "ANKLE_RIGHT"),
	std::make_pair(K4ABT_JOINT_FOOT_RIGHT,    "FOOT_RIGHT"),
	std::make_pair(K4ABT_JOINT_HEAD,          "HEAD"),
	std::make_pair(K4ABT_JOINT_NOSE,          "NOSE"),
	std::make_pair(K4ABT_JOINT_EYE_LEFT,      "EYE_LEFT"),
	std::make_pair(K4ABT_JOINT_EAR_LEFT,      "EAR_LEFT"),
	std::make_pair(K4ABT_JOINT_EYE_RIGHT,     "EYE_RIGHT"),
	std::make_pair(K4ABT_JOINT_EAR_RIGHT,     "EAR_RIGHT")
	};
}

void AzureKinectWrapper::StartLiveLoop()
{
	Shutdown();

	k4a_device_configuration_t device_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	device_config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	device_config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	device_config.camera_fps = K4A_FRAMES_PER_SECOND_30;
	device_config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	device_config.color_resolution = K4A_COLOR_RESOLUTION_1080P;

	mDevice = k4a::device::open(0);
	mDevice.start_cameras(&device_config);

	k4a::calibration sensor_calibration = mDevice.get_calibration(device_config.depth_mode, device_config.color_resolution);

	mCalibration = mDevice.get_calibration(device_config.depth_mode, device_config.color_resolution);
	mTransformation = k4a::transformation(mCalibration);
	mTracker = k4abt::tracker::create(sensor_calibration);

	mThreadLiveLoop = std::thread(&AzureKinectWrapper::LiveLoop, this);
}

void AzureKinectWrapper::LiveLoop()
{
	mFIsThreadAlive = true;
	mFShutdown = false;

	while (!mFShutdown)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));

		k4a::capture sensor_capture;
		if (mDevice.get_capture(&sensor_capture, std::chrono::milliseconds(K4A_WAIT_INFINITE)))
		{
			//std::cout << "Start processing frame " << frame_count << std::endl;

			if (!mTracker.enqueue_capture(sensor_capture))
			{
				// It should never hit timeout when K4A_WAIT_INFINITE is set.
				//std::cout << "Error! Add capture to tracker process queue timeout!" << std::endl;
				Util::OutputDebugMessage("Error! Add capture to tracker process queue timeout!");
				break;
			}

			k4a::image colorImage = sensor_capture.get_color_image();
			k4a::image depthImage = sensor_capture.get_depth_image();
			k4abt::frame body_frame = mTracker.pop_result();
			if (body_frame != nullptr)
			{
				ProcessData(colorImage, depthImage, body_frame);
			}
			else
			{
				//  It should never hit timeout when K4A_WAIT_INFINITE is set.
				//std::cout << "Error! Pop body frame result time out!" << std::endl;
				Util::OutputDebugMessage("Error! Pop body frame result time out!");
				break;
			}
		}
		else
		{
			// It should never hit time out when K4A_WAIT_INFINITE is set.
			//std::cout << "Error! Get depth frame time out!" << std::endl;
			Util::OutputDebugMessage("Error! Get depth frame time out!");
			break;
		}
	}
}

bool AzureKinectWrapper::IsAlive()
{
	return mFIsThreadAlive;
}

ProcessedFrameData AzureKinectWrapper::PopData()
{
	ProcessedFrameData imageData;
	if (mAryProcessedFrameData.size() > 0) {
		std::lock_guard<std::mutex> lock(mMutexProcessData);
		imageData = mAryProcessedFrameData.at(0);
		//mAryProcessedFrameData.erase(mAryProcessedFrameData.begin());
	}
	return imageData;
}

std::string AzureKinectWrapper::GetJointNameById(k4abt_joint_id_t Id)
{
	std::string rtn;

	if (mJointNames.find(Id) != mJointNames.end()) {
		rtn = mJointNames[Id];
	}
	return rtn;
}

void AzureKinectWrapper::ProcessData(k4a::image ColorImage, k4a::image DepthImage, k4abt::frame Frame)
{
	k4a::image bodyImage = Frame.get_body_index_map();

	ProcessedFrameData frameData;

	// process body image
	if (mIsCaptureDepthFrame) {
		int width = bodyImage.get_width_pixels();
		int height = bodyImage.get_height_pixels();
		int channel = bodyImage.get_size() / (width * height);
		int dataLen = width * height * channel;

		frameData.Width = width;
		frameData.Height = height;
		frameData.Channel = channel;
		frameData.Data = new unsigned char[dataLen];
		memcpy_s(frameData.Data, dataLen, bodyImage.get_buffer(), dataLen);
	}

	// process color image
	if (mIsCaptureColorFrame) {
		unsigned char* depthImageDataBody = new unsigned char[DepthImage.get_size()];
		memcpy_s(depthImageDataBody, DepthImage.get_size(), DepthImage.get_buffer(), DepthImage.get_size());
		for (int i = 0; i < bodyImage.get_size(); i++) {
			if (bodyImage.get_buffer()[i] == 255) {
				memset(&depthImageDataBody[i * 2], 0, 2);
			}
		}
		k4a::image depthImageBody = k4a::image::create_from_buffer(DepthImage.get_format(), DepthImage.get_width_pixels(), DepthImage.get_height_pixels(), DepthImage.get_stride_bytes(), depthImageDataBody, DepthImage.get_size(), NULL, NULL);
		k4a::image depthImageBodyTransformed = mTransformation.depth_image_to_color_camera(depthImageBody);

		unsigned char *colorImageBody = new unsigned char[ColorImage.get_size()];
		memcpy_s(colorImageBody, ColorImage.get_size(), ColorImage.get_buffer(), ColorImage.get_size());
		for (int i = 0; i < depthImageBodyTransformed.get_height_pixels(); i++) {
			for (int j = 0; j < depthImageBodyTransformed.get_width_pixels(); j++) {
				int indexDepth = (i * depthImageBodyTransformed.get_width_pixels() + j) * 2;
				int indexColor = (i * depthImageBodyTransformed.get_width_pixels() + j) * 4;
				unsigned char *colorPtrDepth = &(depthImageBodyTransformed.get_buffer()[indexDepth]);
				unsigned char *colorPtrColor = &(colorImageBody[indexColor]);
				if ((colorPtrDepth[0] + colorPtrDepth[1] * 256) == 0) {
					memset(colorPtrColor, 0, 4);
				}
			}
		}
		frameData.ColorData = colorImageBody;

		delete depthImageDataBody;
		depthImageBody.reset();
		depthImageBodyTransformed.reset();
	}
	
	clock_t timeCurrent = clock();

	// process skeleton data
	size_t numBodies = Frame.get_num_bodies();
	for (int i = 0; i < numBodies; i++) {
		ProcessedBodyData bodyData;

		k4abt_body_t body = Frame.get_body(i);
		k4abt_skeleton_t skeleton = Frame.get_body_skeleton(i);

		int confidenceValue = 0;
		for (int boneIdx = 0; boneIdx < mBoneList.size(); boneIdx++) {
			k4abt_joint_id_t joint1 = mBoneList[boneIdx].first;
			k4abt_joint_id_t joint2 = mBoneList[boneIdx].second;

			//if (skeleton.joints[joint1].confidence_level >= K4ABT_JOINT_CONFIDENCE_LOW &&
			//	skeleton.joints[joint2].confidence_level >= K4ABT_JOINT_CONFIDENCE_LOW) {
				bool confidentBone =
					skeleton.joints[joint1].confidence_level >= K4ABT_JOINT_CONFIDENCE_MEDIUM &&
					skeleton.joints[joint2].confidence_level >= K4ABT_JOINT_CONFIDENCE_MEDIUM;
				k4a_float3_t joint1Position = skeleton.joints[joint1].position;
				k4a_float3_t joint2Position = skeleton.joints[joint2].position;
				k4a_quaternion_t quaternion = skeleton.joints[joint1].orientation;

				confidenceValue += confidentBone ? 1 : 0;
				static float millimeterToMeter = 0.001f;

				ProcessedBoneData boneData;
				boneData.RootJointId = joint1;
				boneData.Vertices[0] = Vector3(
					joint1Position.xyz.x * millimeterToMeter,
					joint1Position.xyz.y * millimeterToMeter,
					joint1Position.xyz.z * millimeterToMeter);
				boneData.Vertices[1] = Vector3(
					joint2Position.xyz.x * millimeterToMeter,
					joint2Position.xyz.y * millimeterToMeter,
					joint2Position.xyz.z * millimeterToMeter);
				boneData.Quaternion = Vector4(
					quaternion.wxyz.x,
					quaternion.wxyz.y,
					quaternion.wxyz.z,
					quaternion.wxyz.w
					);
				boneData.IsConfidentBone = confidentBone;

				bodyData.Bones.insert(std::pair<int, ProcessedBoneData>(joint1, boneData));
			}
		//}
		bodyData.ConfidenceValue = confidenceValue;

		frameData.Bodies.push_back(bodyData);
	}

	clock_t timePast = clock() - timeCurrent;
	timeCurrent = clock();

	std::lock_guard<std::mutex> lock(mMutexProcessData);

	if (mAryProcessedFrameData.size() > 0)
	{
		delete mAryProcessedFrameData.at(0).Data;
		delete mAryProcessedFrameData.at(0).ColorData;
		mAryProcessedFrameData.at(0) = frameData;
	}
	else
	{
		mAryProcessedFrameData.push_back(frameData);
	}
	
	if (mAryProcessedFrameData.size() > mProcessedFrameDataLimit) {
		mAryProcessedFrameData.erase(mAryProcessedFrameData.begin());
	}

	timePast = clock() - timeCurrent;
	int aa = 0;
}

void AzureKinectWrapper::Shutdown()
{
	if (mFIsThreadAlive) {
		mFShutdown = true;

		if (mThreadLiveLoop.joinable()) {
			mThreadLiveLoop.join();
		}
	}

	mFIsThreadAlive = false;
	mFShutdown = false;
}

