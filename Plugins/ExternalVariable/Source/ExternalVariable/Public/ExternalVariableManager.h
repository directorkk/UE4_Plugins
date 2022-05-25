// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ExternalVariableFunctionLibrary.h"
#include "Util.h"

#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#include "GameFramework/Actor.h"
#include "ExternalVariableManager.generated.h"

UCLASS()
class EXTERNALVARIABLE_API AExternalVariableManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExternalVariableManager();

	UFUNCTION(BlueprintCallable)
		void LoadSettingFile(FString SettingFileName);
	UFUNCTION(BlueprintCallable)
		void LoadSettingFilesInDirectory(FString DirectoryFullPath);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void FetchExternalVariables(FString SourceName, TMap<FString, FString> &Args);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetExternalVariable(FString SourceName, FString Key, FString &Value, bool &Result);

private:
	TMap<FString, TMap<FString, FString>> mMapExternalVariable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
