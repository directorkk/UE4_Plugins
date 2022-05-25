// Fill out your copyright notice in the Description page of Project Settings.


#include "Util.h"
using namespace AzureKinectBodyTracking;

Util::Util()
{

}

Util::~Util()
{
}
bool Util::InTheRange(double Subject, double RangeA, double RangeB)
{
	double max, min;

	max = RangeA >= RangeB ? RangeA : RangeB;
	min = RangeA < RangeB ? RangeA : RangeB;

	return (Subject <= max && Subject >= min);
}

std::string Util::wstos(const wchar_t * Data)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string rtn = converter.to_bytes(Data);

	return rtn;
}

std::wstring Util::stows(const char * Data)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring rtn = converter.from_bytes(Data);

	return rtn;
}

void Util::OutputDebugMessage(FString Message)
{
	if (GEngine->GetWorldContexts().Num() != 0) {
		UKismetSystemLibrary::PrintString(GEngine->GetWorldContexts()[0].World(), Message);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}
}

void Util::OutputDebugMessage(const char * Message)
{
	std::wstring wstr = stows(Message);
	FString uestr = wstr.c_str();

	OutputDebugMessage(uestr);
}

void Util::OutputDebugMessage(std::string Message)
{
	OutputDebugMessage(Message.c_str());
}
