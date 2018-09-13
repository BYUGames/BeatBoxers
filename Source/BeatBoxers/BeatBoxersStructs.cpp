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
	case EInputToken::IE_LightCrouch:
		if (LightCrouch) return true;
		break;
	case EInputToken::IE_LightForward:
		if (LightForward) return true;
		break;
	case EInputToken::IE_LightBack:
		if (LightBack) return true;
		break;
	case EInputToken::IE_Medium:
		if (Medium) return true;
		break;
	case EInputToken::IE_MediumCrouch:
		if (MediumCrouch) return true;
		break;
	case EInputToken::IE_Heavy:
		if (Heavy) return true;
		break;
	case EInputToken::IE_HeavyCrouch:
		if (HeavyCrouch) return true;
		break;
	case EInputToken::IE_Special1:
		if (Special1) return true;
		break;
	case EInputToken::IE_Special2:
		if (Special2) return true;
		break;
	case EInputToken::IE_Special3:
		if (Special3) return true;
		break;
	case EInputToken::IE_Super:
		if (Super) return true;
		break;
	case EInputToken::IE_Jump:
		if (Jump) return true;
		break;
	case EInputToken::IE_Block:
		if (Block) return true;
		break;
	case EInputToken::IE_DashForward:
		if (DashForward) return true;
		break;
	case EInputToken::IE_DashBackward:
		if (DashBackward) return true;
		break;
	case EInputToken::IE_DashCancelForward:
		if (DashCancelForward) return true;
		break;
	case EInputToken::IE_DashCancelBackward:
		if (DashCancelBackward) return true;
		break;
	case EInputToken::IE_QCF:
		if (QCF) return true;
		break;
	case EInputToken::IE_QCB:
		if (QCB) return true;
		break;
	case EInputToken::IE_DashRight:
		if (DashRight) return true;
		break;
	case EInputToken::IE_DashLeft:
		if (DashLeft) return true;
		break;
	case EInputToken::IE_DashCancelRight:
		if (DashCancelRight) return true;
		break;
	case EInputToken::IE_DashCancelLeft:
		if (DashCancelLeft) return true;
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

FMusicBalanceParams FMusicBalanceParams::operator+(const FMusicBalanceParams& rhs)
{
	FMusicBalanceParams res;
	res.bIsMaestraOn = bIsMaestraOn || rhs.bIsMaestraOn;
	res.bIsRiffOn = bIsRiffOn || rhs.bIsRiffOn;
	return res;
}

FMusicBalanceParams& FMusicBalanceParams::operator+=(const FMusicBalanceParams& rhs)
{
	bIsMaestraOn |= rhs.bIsMaestraOn;
	bIsRiffOn |= rhs.bIsRiffOn;
	return *this;
}


int GetRPSWinner(ERPSType a, ERPSType b)
{
	if (a == b) return 0;
	if (a == ERPSType::RPS_None) return 2;
	if (b == ERPSType::RPS_None) return 1;
	switch (a)
	{
	case ERPSType::RPS_Attack:
		switch (b)
		{
		case ERPSType::RPS_Block:
			return 2;
		case ERPSType::RPS_Grab:
			return 1;
		}
	case ERPSType::RPS_Grab:
		switch (b)
		{
		case ERPSType::RPS_Attack:
			return 2;
		case ERPSType::RPS_Block:
			return 1;
		}
	case ERPSType::RPS_Block:
		switch (b)
		{
		case ERPSType::RPS_Grab:
			return 2;
		case ERPSType::RPS_Attack:
			return 1;
		}
	}
	return 0;
}
