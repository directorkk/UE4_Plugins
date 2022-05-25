// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalVariableFunctionLibrary.h"
using namespace ExternalVariable;

void UExternalVariableFunctionLibrary::FetchCommandLineArgs(TMap<FString, FString> &Args)
{
	FString arg = FCommandLine::Get();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *arg);

	FString splitPattern = "-";
	FString leftStr, rightStr;
	int count = 0;
	bool lastOne = false;
	TCHAR lastChar;
	while (!lastOne) {
		if (!arg.Split(splitPattern, &leftStr, &rightStr)) {
			lastOne = true;
		}

		if (FindCharCountOfString(leftStr, '"') % 2 == 1) {
			// parameter string has char '-', ex: -arg="floor-1"
			FString tmpSplitPattern = "\"";
			FString tmpLStr, tmpRStr;
			rightStr.Split(tmpSplitPattern, &tmpLStr, &tmpRStr);
			leftStr.Append("-");
			leftStr.Append(tmpLStr);
			leftStr.Append("\"");
			rightStr = tmpRStr;
		}

		arg = FString(rightStr);

		if (lastOne) {
			leftStr = arg;
		}

		leftStr = leftStr.Trim();
		if (!leftStr.IsEmpty()) {
			FString tmpSplitPattern = "=";
			FString tmpLStr, tmpRStr;
			FString key, value;
			leftStr.Split(tmpSplitPattern, &tmpLStr, &tmpRStr);

			if (tmpLStr.IsEmpty()) {
				// for unknown reason, there would be a untrimable space at last of string except last one
				key = leftStr;
				if (!lastOne) {
					lastChar = key[key.Len() - 1];
					key = key.Left(key.Len() - 1);
				}
				//value = "WithoutVal" + FString::FromInt(count++);
				value = "";
			}
			else {
				key = tmpLStr;
				value = tmpRStr;
				value = value.Left(value.Len() - 1);
			}
			Args.Add(key.ToLower().Trim(), value.Trim());
		}
	}
}

void UExternalVariableFunctionLibrary::FetchFullCommandLineArgs(FString& Arg)
{
	Arg = FCommandLine::Get();
}

void UExternalVariableFunctionLibrary::GetCommandLineArg(const FString Key, FString &Value, bool &Result)
{
	TMap<FString, FString> commandLineArgs;
	FetchCommandLineArgs(commandLineArgs);

	FString *result = commandLineArgs.Find(Key);
	Result = result != nullptr;
	if (Result) {
		Value = *result;
	}
}

void UExternalVariableFunctionLibrary::FetchSettingFileArgs(const FString SettingFileName, TMap<FString, FString> &Args)
{
	FString output;
	bool result = FFileHelper::LoadFileToString(output, *SettingFileName);
	if (result) {
		TSharedPtr<FJsonObject> jsonObject;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(output);
		if (FJsonSerializer::Deserialize(reader, jsonObject)) {
			TSharedPtr<FJsonObject> settingObject = jsonObject->GetObjectField("setting");
			TArray<FString> settingObjectKeys;
			settingObject->Values.GetKeys(settingObjectKeys);
			for (int i = 0; i < settingObjectKeys.Num(); i++) {
				FString key = settingObjectKeys[i];
				FString val = settingObject->Values[key]->AsString();
				Args.Add(key, val);
			}
		}

	}
}

void UExternalVariableFunctionLibrary::GetSettingFileArg(const FString SettingFileName, const FString Key, FString &Value, bool &Result)
{
	TMap<FString, FString> settingFileArgs;
	FetchSettingFileArgs(SettingFileName, settingFileArgs);

	FString *result = settingFileArgs.Find(Key);
	Result = result != nullptr;
	if (Result) {
		Value = *result;
	}
}

void UExternalVariableFunctionLibrary::GetGameDirectory(FString &Path)
{
	Path = FPaths::GameDir();
}

int UExternalVariableFunctionLibrary::FindCharCountOfString(FString Str, TCHAR InChar)
{
	int count = 0;

	FString target = FString(Str);
	FString splitPattern = FString() + InChar;
	FString leftStr, rightStr;
	while (target.Split(splitPattern, &leftStr, &rightStr))
	{
		target = FString(rightStr);
		count++;
	}

	return count;
}
