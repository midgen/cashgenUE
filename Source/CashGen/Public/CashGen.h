// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "ModuleManager.h"

#define Msg(Text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT(Text));


DECLARE_STATS_GROUP(TEXT("CashGen"), STATGROUP_CashGenStat, STATCAT_Advanced);

class CASHGEN_API FCashGen : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	bool HandleSettingsSaved();
};
