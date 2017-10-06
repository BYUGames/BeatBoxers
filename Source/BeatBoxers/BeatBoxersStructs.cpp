// copyright 2017 BYU Animation

#include "BeatBoxersStructs.h"

bool FMovement::IsValid() const
{
	return Duration >= 0 && (Delta.X != 0 || Delta.Y != 0 || Delta.Z != 0);
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
	case EInputToken::IE_Punch:
		if (Punch) return true;
		break;
	case EInputToken::IE_Kick:
		if (Kick) return true;
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
	default:
		return false;
		break;
	}
	return false;
}

AMoveState::AMoveState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MoveWindows = TArray<FMoveWindow>();
	PossibleTransitions = TArray< TSubclassOf<AMoveState> >();
}

FString FMovement::ToString() const
{
	return FString::Printf(TEXT("Delta={%s} Duration=%f IsRelative=%s"), *Delta.ToString(), Duration, (IsRelativeToAttackerFacing) ? TEXT("TRUE") : TEXT("FALSE"));
}