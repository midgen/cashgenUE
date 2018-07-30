// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CashGen.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "CGSettings.h"
//#include "cashgenPrivatePCH.h"

#define LOCTEXT_NAMESPACE "FCashGen"

void FCashGen::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "CashGen",
			LOCTEXT("CashGenSettingsName", "CashGen"),
			LOCTEXT("CashGenSettingsDescription", "Configure the CashGen Plugin"),
			GetMutableDefault<UCGSettings>()
		);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FCashGen::HandleSettingsSaved);
		}
	}
}

void FCashGen::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// unregister settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "CashGen");
	}
}

bool FCashGen::HandleSettingsSaved()
{
	//RestartServices();

	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCashGen, CashGen)