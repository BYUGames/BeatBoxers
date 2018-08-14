// copyright 2017 BYU Animation
using System.IO;
using UnrealBuildTool;

public class BeatBoxers : ModuleRules
{
	public BeatBoxers(ReadOnlyTargetRules Target) : base(Target)
	{
        //var basePath = FPaths.GameContentDir();
        //basePath= basePath + "Source";
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EngineSettings","Paper2D" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        

       // string ModulePath = ModuleDirectory; //this return your project folder
       // string ThirdParty = Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); // now get the ThirdParty directory

        //string LibraryPath = Path.Combine(ThirdParty, "FMOD", "lib"); // my lib dir
       // string IncludePath = Path.Combine(ThirdParty, "FMOD", "inc"); // my include dir

        //include the stuff, 
        //PublicLibraryPaths.Add(LibraryPath); // add your library dir
        //PublicIncludePaths.Add(IncludePath); // add your include dir
        //PublicAdditionalLibraries.Add("fmodexL64_vc.lib"); // link libraries
        //PublicAdditionalLibraries.Add(LibraryPath + "/opencv_world300.lib");







        // PublicAdditionalLibraries.Add(@"E:\downloads\BeatDetectorForGames-master\BeatDetectorForGames-master\BeatDetector\BeatDetectorC++Version\FMOD\lib\fmodex_vc.lib");
        //PublicIncludePaths.Add(basePath + "Public");
        // PublicLibraryPaths.Add(basePath + ".."+ "Build");

        // PublicAdditionalLibraries.Add("fmodex_vc.lib");//FMOD/lib/
        // PublicIncludePaths.AddRange(new string[] { "FMOD/inc" });
        // PublicIncludePaths.AddRange(new string[] { "FMOD/lib" });
        //PublicAdditionalLibraries.Add("fmodex_vc.lib");
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
