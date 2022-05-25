// Fill out your copyright notice in the Description page of Project Settings.


#include "AzureKinectHandler.h"

// Sets default values
AAzureKinectHandler::AAzureKinectHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mpAzureKinectWrapper = nullptr;
}

void AAzureKinectHandler::Shutdown()
{
	if (mpAzureKinectWrapper != nullptr) {
		mpAzureKinectWrapper->Shutdown();
		delete mpAzureKinectWrapper;

		mpAzureKinectWrapper = nullptr;
	}
}

// Called when the game starts or when spawned
void AAzureKinectHandler::BeginPlay()
{
	Super::BeginPlay();
}

void AAzureKinectHandler::Start()
{
	mpAzureKinectWrapper = new AzureKinectWrapper();
	mpAzureKinectWrapper->StartLiveLoop();
}

void AAzureKinectHandler::SetupCaptureSetting(bool IsCaptureDepthFrame, bool IsCaptureColorFrame)
{
	if (mpAzureKinectWrapper == nullptr) {
		return;
	}
	mIsCaptureDepthFrame = IsCaptureDepthFrame;
	mIsCaptureColorFrame = IsCaptureColorFrame;
	mpAzureKinectWrapper->SetupCaptureSetting(IsCaptureDepthFrame, IsCaptureColorFrame);
}

void AAzureKinectHandler::PopFrameData(bool IsDepthTextureUpdate, UTexture2D *DepthTexture, bool IsColorTextureUpdate, UTexture2D *ColorTexture, bool &Result, FKinectFrameData &FrameData)
{
	if (mpAzureKinectWrapper == nullptr) {
		Result = false;
		return;
	}

	ProcessedFrameData frameData = mpAzureKinectWrapper->PopData();
	Result = frameData.Data != nullptr;
	if (Result) {
		FrameData.FromWrapper(frameData);
		if (IsDepthTextureUpdate && mIsCaptureDepthFrame) {
			UpdateDepthTexture(DepthTexture, frameData);
		}
		if (IsColorTextureUpdate && mIsCaptureColorFrame) {
			UpdateColorTexture(ColorTexture, frameData);
		}
		//delete frameData.Data;
	}
}

void AAzureKinectHandler::UpdateDepthTexture(UTexture2D *DepthTexture, ProcessedFrameData FrameData)
{
	int widthFrame = FrameData.Width;
	int heightFrame = FrameData.Height;
	int channelFrame = FrameData.Channel;

	FTexture2DMipMap *depthTextureMip = &(DepthTexture->PlatformData->Mips[0]);
	FByteBulkData *rawImageData = &depthTextureMip->BulkData;
	int widthTex = depthTextureMip->SizeX;
	int heightTex = depthTextureMip->SizeY;
	int channelTex = DepthTexture->HasAlphaChannel() ? 4 : 3;
	unsigned char *imageData = (unsigned char*)rawImageData->Lock(LOCK_READ_WRITE);
	if (imageData != nullptr) {
		memset(imageData, 0, widthTex * heightTex * channelTex);
		unsigned char idFrame = 0;
		for (int i = 0; i < heightTex; i++) {
			for (int j = 0; j < widthTex; j++) {
				int indexTex = ((i * widthTex) + j) * channelTex;
				int indexFrame = ((i * widthFrame) + j) * channelFrame;
				unsigned char *colorPtrTex = &(imageData[indexTex]);
				unsigned char *colorPtrFrame = &(FrameData.Data[indexFrame]);
				//if (colorPtrFrame[0] < 3) {
				//	colorPtrTex[colorPtrFrame[0]] = 255;
				//}
				colorPtrTex[0] = colorPtrFrame[0] == 255 ? 255 : 0;
				colorPtrTex[1] = colorPtrFrame[0] == 255 ? 255 : 0;
				colorPtrTex[2] = colorPtrFrame[0] == 255 ? 255 : 0;
				if (channelTex == 4) {
					colorPtrTex[3] = 255;
				}
			}
		}
// 		bool aa = false;
// 		if (aa) {
// 			unsigned char bmpHeader[] = {
// 				0x42, 0x4D, 0x36, 0xE0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
// 				0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
// 				0x00, 0x00, 0x00, 0xE0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// 				0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// 			};
// 
// 			std::ofstream output;
// 			output.open("D:\\out.txt", std::ios::binary);
// 			output.write((const char *)bmpHeader, 54);
// 			output.write((const char *)FrameData.Data, widthFrame * heightFrame);
// 			output.close();
// 		}
	}

	rawImageData->Unlock();
	DepthTexture->UpdateResource();
}

void AAzureKinectHandler::UpdateColorTexture(UTexture2D *ColorTexture, ProcessedFrameData FrameData)
{
	int widthFrame = 1920;
	int heightFrame = 1080;
	int channelFrame = 4;

	FTexture2DMipMap *colorTextureMip = &(ColorTexture->PlatformData->Mips[0]);
	FByteBulkData *rawImageData = &colorTextureMip->BulkData;
	int widthTex = colorTextureMip->SizeX;
	int heightTex = colorTextureMip->SizeY;
	int channelTex = ColorTexture->HasAlphaChannel() ? 4 : 3;
	unsigned char *imageData = (unsigned char*)rawImageData->Lock(LOCK_READ_WRITE);
	if (imageData != nullptr) {
		memcpy_s(imageData, widthTex * heightTex * channelTex, FrameData.ColorData, widthTex * heightTex * channelTex);
		
		/*memset(imageData, 0, widthTex * heightTex * channelTex);
		unsigned char idFrame = 0;
		for (int i = 0; i < heightTex; i++) {
			for (int j = 0; j < widthTex; j++) {
				int indexTex = ((i * widthTex) + j) * channelTex;
				int indexFrame = ((i * widthFrame) + j) * channelFrame;
				unsigned char *colorPtrTex = &(imageData[indexTex]);
				unsigned char *colorPtrFrame = &(FrameData.ColorData[indexFrame]);
				//if (colorPtrFrame[0] < 3) {
				//	colorPtrTex[colorPtrFrame[0]] = 255;
				//}
				colorPtrTex[0] = FrameData.ColorData[0];
				colorPtrTex[1] = FrameData.ColorData[1];
				colorPtrTex[2] = FrameData.ColorData[2];
				if (channelTex == 4) {
					colorPtrTex[3] = 255;
				}
			}
		}*/
		bool aa = false;
		if (aa) {
			unsigned char bmpHeader[] = {
				0x42, 0x4D, 0x36, 0xEC, 0x5E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
				0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x38, 0x04, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};

			unsigned char *bmpData = new unsigned char[widthFrame * heightFrame * 3];
			for (int i = 0; i < heightFrame; i++) {
				for (int j = 0; j < widthFrame; j++) {
					unsigned char *colorPtrFrame = &FrameData.ColorData[(i * widthFrame + j) * 4];
					unsigned char *colorPtrBmp = &bmpData[(i * widthFrame + j) * 3];
					memcpy_s(colorPtrBmp, 3, colorPtrFrame, 3);
				}
			}

			std::ofstream output;
			output.open("D:\\out.bmp", std::ios::binary);
			output.write((const char *)bmpHeader, 54);
			output.write((const char *)bmpData, widthFrame * heightFrame*3);
			output.close();

			delete bmpData;
		}
	}

	rawImageData->Unlock();
	ColorTexture->UpdateResource();
}

void AAzureKinectHandler::CreateTexture2DTransient(int Width, int Height, int Amount, TArray<UTexture2D*> &OutputTextures)
{
	for (int i = 0; i < Amount; i++) {
		UTexture2D *texture = UTexture2D::CreateTransient(Width, Height);
		OutputTextures.Add(texture);
	}
}


void AAzureKinectHandler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	Shutdown();
}

// Called every frame
void AAzureKinectHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

