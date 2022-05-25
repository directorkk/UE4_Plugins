// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalVariableManager.h"
using namespace ExternalVariable;

// Sets default values
AExternalVariableManager::AExternalVariableManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AExternalVariableManager::LoadSettingFile(FString SettingFileName)
{
	TMap<FString, FString> settingFileArgs;
	UExternalVariableFunctionLibrary::FetchSettingFileArgs(SettingFileName, settingFileArgs);

	FString settingFileExt = FPaths::GetExtension(SettingFileName, true);
	FString cleanSettingFileName = FPaths::GetCleanFilename(SettingFileName).Replace(*settingFileExt, L"");
	mMapExternalVariable.Add(cleanSettingFileName, settingFileArgs);

	Util::OutputDebugMessage("Load setting file: " + SettingFileName);
}

void AExternalVariableManager::LoadSettingFilesInDirectory(FString DirectoryFullPath)
{
	if (FPaths::DirectoryExists(DirectoryFullPath)) {
		IFileManager& FileManager = IFileManager::Get();

		TArray<FString> settingFileList;
		FileManager.FindFiles(settingFileList, *DirectoryFullPath, L"json");
		for (int i = 0; i < settingFileList.Num(); i++) {
			FString settingFileName = settingFileList[i];
			LoadSettingFile(FPaths::Combine(DirectoryFullPath, settingFileName));
		}
	}
}

void AExternalVariableManager::FetchExternalVariables(FString SourceName, TMap<FString, FString> &Args)
{
	TMap<FString, FString> *result = mMapExternalVariable.Find(SourceName);
	if (result != nullptr) {
		Args.Append(*result);
	}
}

void AExternalVariableManager::GetExternalVariable(FString SourceName, FString Key, FString &Value, bool &Result)
{
	TMap<FString, FString> *result = mMapExternalVariable.Find(SourceName);
	if (result != nullptr) {
		FString *valResult = result->Find(Key);
		if (valResult != nullptr) {
			Result = true;
			Value = *valResult;
		}
	}
}

// Called when the game starts or when spawned
void AExternalVariableManager::BeginPlay()
{
	Super::BeginPlay();

	TMap<FString, FString> commandLineArgs;
	UExternalVariableFunctionLibrary::FetchCommandLineArgs(commandLineArgs);
	mMapExternalVariable.Add("COMMAND_LINE", commandLineArgs);
}

// Called every frame
void AExternalVariableManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

