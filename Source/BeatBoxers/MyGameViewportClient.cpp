// copyright 2017 BYU Animation

#include "MyGameViewportClient.h"
#include "Engine/Console.h"
#include "Implementation/BBPlayerController.h"


/*
bool UMyGameViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	// Propagate keyboard events to all players
	UEngine* const Engine = GetOuterUEngine();
	int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
	bool bRetVal = false;
	for (int32 i = 0; i < NumPlayers; i++)
	{
		bRetVal = Super::InputKey(Viewport, i, Key, EventType, AmountDepressed, bGamepad) || bRetVal;
	}

	return bRetVal;
}

bool UMyGameViewportClient::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	// Propagate keyboard events to all players
	UEngine* const Engine = GetOuterUEngine();
	int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
	bool bRetVal = false;
	for (int32 i = 0; i < NumPlayers; i++)
	{
		bRetVal = Super::InputAxis(Viewport, i, Key, Delta, DeltaTime, NumSamples, bGamepad) || bRetVal;
	}

	return bRetVal;

}
*/