// copyright 2017 BYU Animation

#include "BBPlayerState.h"

ABBPlayerState::ABBPlayerState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxSpecial = 100.f;
}

float ABBPlayerState::GetSpecial() const
{
	return Special;
}

float ABBPlayerState::GetSpecialAsPercent() const
{
	return (MaxSpecial > 0.f) ? Special / MaxSpecial : 1.f;
}

void ABBPlayerState::AddSpecial(float Amount)
{
	Special = FMath::Min(Special + Amount, MaxSpecial);
}

bool ABBPlayerState::UseSpecial(float Amount)
{
	if (Special >= Amount)
	{
		Special -= Amount;
		return true;
	}
	return false;
}