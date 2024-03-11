// Copyright Epic Games, Inc. All Rights Reserved.

#include "IDCGames.h"

#include "ISettingsModule.h"
#include "IDCGamesPlugin_Settings.h"

#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"

#include "LevelEditor.h"
#include "ToolMenus.h"

static const FName IDCGamesTabName("IDCGames");

#define LOCTEXT_NAMESPACE "FIDCGamesModule"

void FIDCGamesModule::StartupModule() {
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
		SettingsModule->RegisterSettings("Project", "Plugins", "IDCGamesPlugin_Settings",
			LOCTEXT("RuntimeSettingsName", "IDC Games"), LOCTEXT("RuntimeSettingsDescription", "Builds Uploader setting"),
			GetMutableDefault<UIDCGamesPlugin_Settings>());
	}
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuExtension("LevelEditor", EExtensionHook::After, NULL, FMenuExtensionDelegate::CreateRaw(this, &FIDCGamesModule::AddMenuEntry));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FIDCGamesModule::ShutdownModule() {
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
		SettingsModule->UnregisterSettings("Project", "Plugins", "IDCGamesPlugin_Settings");
	}
}

void FIDCGamesModule::AddMenuEntry(FMenuBuilder& MenuBuilder) {
	// Create Section
	MenuBuilder.BeginSection("CustomMenu", TAttribute(FText::FromString("IDC Games")));
	// Create a Submenu inside of the Section
	MenuBuilder.AddSubMenu(FText::FromString("Tools"),
		FText::FromString(""),
		FNewMenuDelegate::CreateRaw(this, &FIDCGamesModule::FillSubmenu));
	MenuBuilder.EndSection();
}

void FIDCGamesModule::FillSubmenu(FMenuBuilder& MenuBuilder) {
	MenuBuilder.AddMenuEntry(
		FText::FromString("Open Build Loader Dialog"),
		FText::FromString(""),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FIDCGamesModule::OpenBuildLoaderDialog))
	);
}

void FIDCGamesModule::OpenBuildLoaderDialog() {
	UObject* Blueprint = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, L"/Script/Blutility.EditorUtilityWidgetBlueprint'/IDCGames/IDCBuildUploader.IDCBuildUploader'");
	if (IsValid(Blueprint)) {
		UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(Blueprint);
		if (IsValid(EditorWidget)) {
			UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
			EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FIDCGamesModule, IDCGames)