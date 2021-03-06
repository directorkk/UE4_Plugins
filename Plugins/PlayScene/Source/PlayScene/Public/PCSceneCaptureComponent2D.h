// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlaySceneSlate.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Components/SceneCaptureComponent2D.h"
#include "PCSceneCaptureComponent2D.generated.h"

/**
 * 
 */
UCLASS(hidecategories = (Collision, Object, Physics, SceneComponent), ClassGroup = Rendering, editinlinenew, meta = (BlueprintSpawnableComponent))
class PLAYSCENE_API UPCSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	virtual void Activate(bool bReset) override;

	virtual void OnRegister() override;
	
};
