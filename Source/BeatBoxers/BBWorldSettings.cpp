// copyright 2017 BYU Animation

#include "BBWorldSettings.h"

ABBWorldSettings::ABBWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraBounds.Max = FVector{ 1000.f, 1000.f, 1000.f };
	CameraBounds.Min = FVector{ -1000.f, 0.f, 0.f };
	CameraPadding = FVector2D{ 200.f, 100.f };
	InitialCameraLocation = FVector{ 0, 800, 180 };
	InitialCameraLookAtLocation = FVector{ 0, 0, 180 };
}



