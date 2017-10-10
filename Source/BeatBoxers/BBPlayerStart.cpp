// copyright 2017 BYU Animation

#include "BBPlayerStart.h"

ABBPlayerStart::ABBPlayerStart(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerID = -1;
}

int32 ABBPlayerStart::GetPlayerID()
{
	return PlayerID + 256;
}