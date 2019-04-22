// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class DawitAndGoliathServerTarget : TargetRules   // Change this line as shown previously
{
	public DawitAndGoliathServerTarget(TargetInfo Target) : base(Target)  // Change this line as shown previously
	{
		Type = TargetType.Server;
		ExtraModuleNames.AddRange(new string[] { "DawitAndGoliath" });
	}
}