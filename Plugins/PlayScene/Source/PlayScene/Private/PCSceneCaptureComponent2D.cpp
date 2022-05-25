// Fill out your copyright notice in the Description page of Project Settings.

#include "PCSceneCaptureComponent2D.h"
#include "IPlayScene.h"

void UPCSceneCaptureComponent2D::Activate(bool bReset)
{
	Super::Activate();
	// Assign Render texture to shared module variale
	//IPlayScene::Get().SetTextureRenderTarget2D(this->TextureTarget);
	//FPlaySceneSlate::Initialize();
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *(this->TextureTarget->GetFullName()));
	//int aa = 0;
}

void UPCSceneCaptureComponent2D::OnRegister()
{
	Super::OnRegister();
	// Assign Render texture to shared module variale
	//IPlayScene::Get().SetTextureRenderTarget2D(this->TextureTarget);
}
