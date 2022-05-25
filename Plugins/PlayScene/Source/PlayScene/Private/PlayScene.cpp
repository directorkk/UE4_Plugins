// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PlayScene.h"
#include "PlaySceneStyle.h"
#include "PlaySceneCommands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#if WITH_EDITOR
	#include "LevelEditor.h"
#endif

#include "PlaySceneSlate.h"

#define LOCTEXT_NAMESPACE "FPlaySceneModule"

void FPlaySceneModule::StartupModule()
{
	// Initialize play button ui style
	FPlaySceneStyle::Initialize();
	FPlaySceneStyle::ReloadTextures();

	// Register play capture commands
	FPlaySceneCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);

	// Add play capture button command
	PluginCommands->MapAction(
		FPlaySceneCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPlaySceneModule::PluginButtonClicked),
		FCanExecuteAction());
		

#if WITH_EDITOR
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		
	// Add play capture button to editor
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FPlaySceneModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
#endif
}

void FPlaySceneModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FPlaySceneStyle::Shutdown();

	FPlaySceneCommands::Unregister();

	// Disable PlayScene Window
	FPlaySceneSlate::Shutdown();
}

TWeakObjectPtr<UTextureRenderTarget2D> FPlaySceneModule::GetTextureRenderTarget2D(FString ClientName)
{
	if (mMapTextureRenderTarget2D.Contains(ClientName)) {
		return mMapTextureRenderTarget2D[ClientName];
	}

	return nullptr;

	//return TextureRenderTarget2D;
}

void FPlaySceneModule::SetTextureRenderTarget2D(FString ClientName, UTextureRenderTarget2D * TexRenderTarget2D)
{
	if (!mMapTextureRenderTarget2D.Contains(ClientName)) {
		mMapTextureRenderTarget2D.Add(ClientName, TexRenderTarget2D);
	}
	mMapTextureRenderTarget2D[ClientName] = TexRenderTarget2D;

	//TextureRenderTarget2D = TexRenderTarget2D;
}


void FPlaySceneModule::PluginButtonClicked()
{
	// Init layCapture Window
	//FPlaySceneSlate::Initialize();
}


void FPlaySceneModule::OpenWindow(FString ClientName, FVector2D ClientSize, FVector2D ClientPosition)
{
	FPlaySceneSlate::Initialize(ClientName, ClientSize, ClientPosition);
}


void FPlaySceneModule::CloseWindow(FString ClientName)
{
	FPlaySceneSlate::Shutdown(ClientName);
}


void FPlaySceneModule::CloseAllWindows()
{
	FPlaySceneSlate::Shutdown();
}

void FPlaySceneModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FPlaySceneCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPlaySceneModule, PlayScene)