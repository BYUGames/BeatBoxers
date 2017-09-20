// Fill out your copyright notice in the Description page of Project Settings.

#include "BeatBoxersGameModeBase.h"

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

struct FHitResult ABeatBoxersGameModeBase::TraceHitbox(FMoveHitbox Hitbox, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TArray<class AActor*>& IgnoreActors, class AActor* Source, class AController* SourceController)
{
	//TODO
	return FHitResult();
}