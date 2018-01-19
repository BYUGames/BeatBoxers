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

int ABBPlayerState::GetBeatCombo() const
{
	return BeatCombo;
}

int ABBPlayerState::SetBeatCombo(int Combo)
{
	BeatCombo = Combo;
	return BeatCombo;
}

void ABBPlayerState::TakeDamage(float Amount)
{
	Health -= Amount;
	if (Health < 0)
		Health = 0;
}

float ABBPlayerState::GetHealth()
{
	return Health;
}

void ABBPlayerState::ResetPlayerState()
{
	Special = 0;
	Health = 100.0f;
	BeatCombo = 0;
}