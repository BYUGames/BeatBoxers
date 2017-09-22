// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "FighterWorld.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFighterWorld : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFighterWorld
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Called by FighterStates/Projectiles before performing traces. Allows the game to intercept override requests. */
	virtual EFighterDamageType GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const = 0;

	/** Request made by FighterStates/Projectiles to search for targets. */
	virtual struct FHitResult TraceHitbox(FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) = 0;

	/** Request made to apply damage to an actor. */
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController) = 0;
	
};
