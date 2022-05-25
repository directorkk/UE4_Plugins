#include "PlaySceneSlate.h"
#include "SlateApplication.h"

#include "Engine.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Public/Slate/SceneViewport.h"
#include "SViewport.h"
#include "SToolTip.h"
#include "SWindow.h"

#include "IPlayScene.h"


void FPlaySceneViewportClient::Draw(FViewport * Viewport, FCanvas * Canvas)
{
	// Clear entire canvas
	Canvas->Clear(FLinearColor::Black);

	// Draw SceenCaptureComponent texture to entire canvas
	auto TextRenderTarget2D = IPlayScene::Get().GetTextureRenderTarget2D(mClientName);
	if (TextRenderTarget2D.IsValid() && TextRenderTarget2D->Resource != nullptr)
	{
		FCanvasTileItem TileItem(FVector2D(0, 0), TextRenderTarget2D->Resource, 
			FVector2D(Viewport->GetRenderTargetTexture()->GetSizeX(), Viewport->GetRenderTargetTexture()->GetSizeY()),
			FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Opaque;
		Canvas->DrawItem(TileItem);
	}
}

bool FPlaySceneViewportClient::InputKey(FViewport * Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	return false;
}

bool FPlaySceneViewportClient::InputAxis(FViewport * Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	return false;
}

bool FPlaySceneViewportClient::InputGesture(FViewport * Viewport, EGestureEvent GestureType, const FVector2D & GestureDelta, bool bIsDirectionInvertedFromDevice)
{
	return false;
}


void FPlaySceneViewportClient::SetClientName(FString ClientName)
{
	mClientName = ClientName;
}

void SPlaySceneViewport::Construct(const FArguments& InArgs)
{
	// Create Viewport Widget
	Viewport = SNew(SViewport)
		.IsEnabled(true)
		.EnableGammaCorrection(false)
		.ShowEffectWhenDisabled(false)
		.EnableBlending(true)
		.ToolTip(SNew(SToolTip).Text(FText::FromString("SPlaySceneViewport")));


	// Create Viewport Client
	PlaySceneViewportClient = MakeShareable(new FPlaySceneViewportClient());

	// Create Scene Viewport
	SceneViewport = MakeShareable(new FSceneViewport(PlaySceneViewportClient.Get(), Viewport));

	// Assign SceneViewport to Viewport widget. It needed for rendering
	Viewport->SetViewportInterface(SceneViewport.ToSharedRef());

	// Assing Viewport widget for our custom PlayScene Viewport
	this->ChildSlot
		[
			Viewport.ToSharedRef()
		];
}


void SPlaySceneViewport::SetClientName(FString ClientName)
{
	mClientName = ClientName;
	PlaySceneViewportClient->SetClientName(ClientName);
}

void SPlaySceneViewport::Tick(const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// Call FViewport each engine tick
	SceneViewport->Draw();
}

TSharedPtr<TMap<FString, TSharedPtr<FPlaySceneSlate>>> FPlaySceneSlate::gMapPlaySceneSlate = MakeShareable(new TMap<FString, TSharedPtr<FPlaySceneSlate>>());
//TSharedPtr<FPlaySceneSlate> FPlaySceneSlate::PlaySceneSlate = nullptr;

FPlaySceneSlate::FPlaySceneSlate(FString ClientName, FVector2D ClientSize, FVector2D ClientPosition)
{
	PlaySceneWindowWidth = ClientSize.X;
	PlaySceneWindowHeight = ClientSize.Y;
	PlaySceneWindowName = ClientName;

	// Create SWindow
	PlaySceneWindow = SNew(SWindow)
		.Title(FText::FromString(*ClientName))
		.ScreenPosition(ClientPosition)
		.ClientSize(ClientSize)
		.AutoCenter(EAutoCenter::None)
		.UseOSWindowBorder(true)
		.SaneWindowPlacement(false)
		.SizingRule(ESizingRule::UserSized);

	FSlateApplication::Get().AddWindow(PlaySceneWindow.ToSharedRef());

	// Assign window events delegator
	//InDelegate.BindRaw(this, &FPlaySceneSlate::OnWindowClosed);
	//PlaySceneWindow->SetOnWindowClosed(InDelegate);

	// Create and assign viewport to window
	PlaySceneViewport = SNew(SPlaySceneViewport);
	PlaySceneViewport->SetClientName(ClientName);
	PlaySceneWindow->SetContent(PlaySceneViewport.ToSharedRef());
}

FPlaySceneSlate::~FPlaySceneSlate()
{
	InDelegate.Unbind();
}

void FPlaySceneSlate::Initialize(FString ClientName, FVector2D ClientSize, FVector2D ClientPosition)
{
	if (!FPlaySceneSlate::gMapPlaySceneSlate.IsValid()) {
		FPlaySceneSlate::gMapPlaySceneSlate = MakeShareable(new TMap<FString, TSharedPtr<FPlaySceneSlate>>());
	}

	// Create single instance of PlaySceneSlate
	if (!FPlaySceneSlate::gMapPlaySceneSlate->Contains(ClientName))
	{
		//PlaySceneSlate = MakeShareable(new FPlaySceneSlate(ClientWidth, ClientHeight));
		FPlaySceneSlate::gMapPlaySceneSlate->Add(ClientName, MakeShareable(new FPlaySceneSlate(ClientName, ClientSize, ClientPosition)));
	}
}

void FPlaySceneSlate::Shutdown(FString ClientName)
{
	if (!FPlaySceneSlate::gMapPlaySceneSlate.IsValid()) {
		FPlaySceneSlate::gMapPlaySceneSlate = MakeShareable(new TMap<FString, TSharedPtr<FPlaySceneSlate>>());
	}

	if (FPlaySceneSlate::gMapPlaySceneSlate->Contains(ClientName))
	{
		TSharedPtr<FPlaySceneSlate> PlaySceneSlate = *FPlaySceneSlate::gMapPlaySceneSlate->Find(ClientName);
		FSlateApplication::Get().DestroyWindowImmediately(PlaySceneSlate->PlaySceneWindow.ToSharedRef());
		//PlaySceneSlate.Reset();
		FPlaySceneSlate::gMapPlaySceneSlate->Remove(ClientName);
	}
}


void FPlaySceneSlate::Shutdown()
{
	if (!FPlaySceneSlate::gMapPlaySceneSlate.IsValid()) {
		FPlaySceneSlate::gMapPlaySceneSlate = MakeShareable(new TMap<FString, TSharedPtr<FPlaySceneSlate>>());
	}

	if (FPlaySceneSlate::gMapPlaySceneSlate->Num() != 0) {
		TArray<FString> keys;
		FPlaySceneSlate::gMapPlaySceneSlate->GetKeys(keys);
		for (int i = 0; i < keys.Num(); i++) {
			FPlaySceneSlate::Shutdown(keys[i]);
		}
	}
}

void FPlaySceneSlate::OnWindowClosed(const TSharedRef<SWindow>& Window)
{
	//FPlaySceneSlate::Shutdown(PlaySceneWindowName);
}

