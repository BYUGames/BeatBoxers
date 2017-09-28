// copyright 2017 BYU Animation

#include "BeatBoxersStructs.h"

bool FMovement::IsValid() const
{
	return Duration > 0 && (Delta.X != 0 || Delta.Y != 0 || Delta.Z != 0);
}