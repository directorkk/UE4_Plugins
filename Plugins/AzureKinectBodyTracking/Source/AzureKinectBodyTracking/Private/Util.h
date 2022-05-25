// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"

#include <string>
#include <locale>
#include <codecvt>

namespace AzureKinectBodyTracking {
	/**
	 *
	 */
	class Util
	{
	public:
		Util();
		~Util();

		static bool InTheRange(double Subject, double RangeA, double RangeB);

		static std::string wstos(const wchar_t* Data);
		static std::wstring stows(const char* Data);

		static void OutputDebugMessage(FString Message);
		static void OutputDebugMessage(const char* Message);
		static void OutputDebugMessage(std::string Message);
	};

}