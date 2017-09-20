// copyright 2017 BYU Animation

using UnrealBuildTool;
using System.Collections.Generic;

public class BeatBoxersEditorTarget : TargetRules
{
	public BeatBoxersEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "BeatBoxers" } );
	}
}
