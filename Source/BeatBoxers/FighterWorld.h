// Fill out your copyright notice in the Description page of Project Settings.

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

	/** Request made by FighterStates/Projectiles to attempt to impact/damage the world. Damages and effects will be applied by the world, hit result should be used to add actors to the ignore list. */
	virtual struct FHitResult TraceHitbox(FMoveHitbox Hitbox, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TArray<class AActor*>& IgnoreActors, class AActor* Source, class AController* SourceController) = 0;
	
};
