// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class cashgenUE : ModuleRules
{
	public cashgenUE(TargetInfo Target)
	{
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent" });

        string LibrariesPath = Path.Combine(ThirdPartyPath, "libnoise", "libraries");

        PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libnoise.lib"));


        PublicIncludePaths.AddRange(new string[] { "ProceduralMeshComponent/Public", "ProceduralMeshComponent/Classes", Path.Combine(ThirdPartyPath, "libnoise", "include") });

        PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }

        
    }

 

    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
}
