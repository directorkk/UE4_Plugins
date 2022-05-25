// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Json.h"

#include "Util.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExternalVariableFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EXTERNALVARIABLE_API UExternalVariableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		static void FetchCommandLineArgs(TMap<FString, FString> &Args);
	UFUNCTION(BlueprintCallable)
		static void FetchFullCommandLineArgs(FString& Arg);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void GetCommandLineArg(const FString Key, FString &Value, bool &Result);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void FetchSettingFileArgs(const FString SettingFileName, TMap<FString, FString> &Args);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void GetSettingFileArg(const FString SettingFileName, const FString Key, FString &Value, bool &Result);

	UFUNCTION(BlueprintCallable)
		static void GetGameDirectory(FString &Path);

private:
	static int FindCharCountOfString(FString Str, TCHAR InChar);

};
