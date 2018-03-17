// copyright 2017 BYU Animation

#include "BBWorldSettings.h"

ABBWorldSettings::ABBWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraBounds.Max = FVector{ 1000.f, 1000.f, 1000.f };
	CameraBounds.Min = FVector{ -1000.f, 100.f, 125.f };
	InitialCameraLocation = FVector{ 0, 1000, 180 };
	InitialCameraLookAtLocation = FVector{ 0, 0, 210 };
	CameraFocus = FVector2D{ 1100.f, 600.f };
	PlayerCameraPadding = FVector2D{ 300.f, 500.f };
}



