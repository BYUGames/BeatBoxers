// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFighterWorld.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoloStartEvent, AActor*, ActorSoloing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSoloEndEvent);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
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
	virtual struct FHitResult TraceHitbox(FVector Source, FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) = 0;

	/** Request made to apply damage to an actor. */
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController) = 0;

	/** Attempts to move the actor. Returns -1 on error, 1 if actor was pressed against a wall, 0 otherwise. */
	virtual int ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement) = 0;

	virtual void StartSolo(TWeakObjectPtr<AActor> OneSoloing) = 0;

	virtual void EndSolo() = 0;

	virtual FSoloStartEvent& GetOnSoloStartEvent() = 0;

	virtual FSoloEndEvent& GetOnSoloEndEvent() = 0;

	/** Hook for grid positioning to be enforced. Called when movement ends. */
	virtual void AdjustLocation(AActor* ActorToAdjust) = 0;

	virtual UObject* GetMusicBox() = 0;

	virtual bool IsOnBeat(float Accuracy) = 0;
};
