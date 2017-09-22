// copyright 2017 BYU Animation

#include "BeatBoxersGameModeBase.h"
#include "FighterCharacter.h"
#include "BBPlayerController.h"

ABeatBoxersGameModeBase::ABeatBoxersGameModeBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AFighterCharacter::StaticClass();
	PlayerControllerClass = ABBPlayerController::StaticClass();
}

EFighterDamageType ABeatBoxersGameModeBase::GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const
{
	//TODO
	return DesiredOverride;
}

struct FHitResult ABeatBoxersGameModeBase::TraceHitbox(FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors)
{
	//TODO
	return FHitResult();
}

EHitResponse ABeatBoxersGameModeBase::HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController)
{
	//TODO
	return EHitResponse::HE_Hit;
}
