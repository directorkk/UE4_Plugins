// Fill out your copyright notice in the Description page of Project Settings.


#include "URG_04LX_UG01_Wrapper.h"

URG_04LX_UG01_Wrapper::URG_04LX_UG01_Wrapper()
{
	Clear();
}

URG_04LX_UG01_Wrapper::~URG_04LX_UG01_Wrapper()
{
	if (mURG != nullptr) {
		delete mURG;
	}
}

void URG_04LX_UG01_Wrapper::Clear()
{
	mURG = nullptr;
	mIsOpen = false;
	mFShutdown = false;
	mScanRangeMin = -90.0f;
	mScanRangeMax = 90.0f;
}

bool URG_04LX_UG01_Wrapper::OpenURG(float aScanRangeMin, float aScanRangeMax, qrk::Lidar::connection_type_t aConnectionType, const char* aDevice_or_ip_name, long aBaudrate_or_port_number)
{
	if (mURG == nullptr) {
		mURG = new qrk::Urg_driver();
	}

	//Connection_information information(argc, argv);
	const char* device_or_ip_name = aDevice_or_ip_name;
	long baudrate_or_port_number = aBaudrate_or_port_number;
	qrk::Lidar::connection_type_t connection_type = aConnectionType;
	// Connects to the sensor
	if (!mURG->open(device_or_ip_name,
		baudrate_or_port_number,
		connection_type)) {
		//cout << "Urg_driver::open(): "<< device_or_ip_name << ": " << urg.what() << endl;
		return false;
	}
	// Gets measurement data
#if 1
	// Case where the measurement range (start/end steps) is defined
	SetScanRange(aScanRangeMin, aScanRangeMax);
#endif
	mURG->start_measurement(qrk::Urg_driver::Distance, qrk::Urg_driver::Infinity_times, 0);
#if defined(URG_MSC)
	getchar();
#endif
	mIsOpen = true;
	return true;
}

void URG_04LX_UG01_Wrapper::StopURG()
{
	if (mIsOpen)
	{
		mURG->stop_measurement();
		mURG->close();
		mIsOpen = false;
	}
}

void URG_04LX_UG01_Wrapper::ShutDown()
{
	mFShutdown = true;
}

void URG_04LX_UG01_Wrapper::UpdateDegreeAndDistance()
{
	std::vector<long> data;
	long time_stamp = 0;
	if (!mURG->get_distance(data, &time_stamp))
	{
		return;
	}
	std::lock_guard<std::mutex> lock(mMutexDegreeAndDistance);
	for (float j = mScanRangeMin; j <= mScanRangeMax; j += 0.5f)
	{
		int front_index = mURG->step2index(mURG->deg2step(j));
		std::ostringstream oss;
		oss << j;
		if (mDegreeAndDistance.find(oss.str()) != mDegreeAndDistance.end())
		{
			mDegreeAndDistance[oss.str()] = data[front_index];
		}
		else
		{
			mDegreeAndDistance.insert(std::pair<std::string, float>(oss.str(), data[front_index]));
		}
	}
}

void URG_04LX_UG01_Wrapper::SetScanRange(float aScanRangeMin, float aScanRangeMax)
{
	mScanRangeMin = aScanRangeMin;
	mScanRangeMax = aScanRangeMax;
	mURG->set_scanning_parameter(mURG->deg2step(mScanRangeMin), mURG->deg2step(mScanRangeMax), 0);
}

void URG_04LX_UG01_Wrapper::URGLiveLoop()
{
	while (!mFShutdown)
	{
		UpdateDegreeAndDistance();
		//std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
}

std::map<std::string, float> URG_04LX_UG01_Wrapper::GetDegreeAndDistance()
{
	std::lock_guard<std::mutex> lock(mMutexDegreeAndDistance);
	//std::map<std::string, float> rtn;
	//std::copy(mDegreeAndDistance.begin(), mDegreeAndDistance.end(), rtn);
	return mDegreeAndDistance;
}
