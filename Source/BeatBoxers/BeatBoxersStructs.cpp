// copyright 2017 BYU Animation

#include "BeatBoxersStructs.h"

bool FMovement::IsValid() const
{
	return Duration >= 0 && (Delta.X != 0 || Delta.Y != 0);
}

bool FStanceFilter::FilterStance(EStance Stance) const
{
	switch (Standing)
	{
	case EFilter::FE_Prohibited:
		if (Stance == EStance::SE_Standing)
		{
			return false;
		}
		break;
	case EFilter::FE_Required:
		if (Stance != EStance::SE_Standing)
		{
			return false;
		}
		break;
	}
	switch (Crouching)
	{
	case EFilter::FE_Prohibited:
		if (Stance == EStance::SE_Crouching)
		{
			return false;
		}
		break;
	case EFilter::FE_Required:
		if (Stance != EStance::SE_Crouching)
		{
			return false;
		}
		break;
	}
	switch (Jumping)
	{
	case EFilter::FE_Prohibited:
		if (Stance == EStance::SE_Jumping)
		{
			return false;
		}
		break;
	case EFilter::FE_Required:
		if (Stance != EStance::SE_Jumping)
		{
			return false;
		}
		break;
	}

	return true;
}

bool FInputTokenBools::FilterInputToken(EInputToken Token) const
{
	if (Any) return true;

	switch (Token)
	{
	case EInputToken::IE_None:
		return false;
		break;
	case EInputToken::IE_Light:
		if (Light) return true;
		break;
	case EInputToken::IE_Medium:
		if (Medium) return true;
		break;
	case EInputToken::IE_Heavy:
		if (Heavy) return true;
		break;
	case EInputToken::IE_Jump:
		if (Jump) return true;
		break;
	case EInputToken::IE_DashLeft:
		if (DashLeft) return true;
		break;
	case EInputToken::IE_DashRight:
		if (DashRight) return true;
		break;
	case EInputToken::IE_ForwardLight:
		if (ForwardLight) return true;
		break;
	case EInputToken::IE_UpperLight:
		if (UpperLight) return true;
		break;
	case EInputToken::IE_CrouchLight:
		if (CrouchLight) return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

FString FMovement::ToString() const
{
	return FString::Printf(TEXT("Delta={%s} Duration=%f IsRelative=%s"), *Delta.ToString(), Duration, (IsRelativeToAttackerFacing) ? TEXT("TRUE") : TEXT("FALSE"));
}

FMovement FMovement::operator-()
{
	FMovement Result = *this;
	Result.Delta = -Delta;
	return Result;
}

FMovement FMovement::operator*(float f)
{
	FMovement Result = *this;
	Result.Delta *= f;
	return Result;
}

bool FEffects::IsValid() const
{
	return ParticleSystem != nullptr || SoundCue != nullptr;
}