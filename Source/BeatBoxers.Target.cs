// copyright 2017 BYU Animation

using UnrealBuildTool;
using System.Collections.Generic;

public class BeatBoxersTarget : TargetRules
{
	public BeatBoxersTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "BeatBoxers" } );
	}
}
