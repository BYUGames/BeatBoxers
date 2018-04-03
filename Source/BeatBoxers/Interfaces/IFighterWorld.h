// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFighterWorld.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRoundStartEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatStartEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFightStartedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeatWindowCloseEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoundEndEvent, int, WinnerIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMatchEndEvent, int, WinnerIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoloStartEvent, AActor*, ActorSoloing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerBeatComboChangedEvent, APlayerController*, PlayerController, int, BeatCombo);
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
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, bool grab) = 0;

	/** Attempts to move the actor. Returns -1 on error, 1 if actor was pressed against a wall, 0 otherwise. */
	virtual int ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement) = 0;

	virtual void StartSolo(TWeakObjectPtr<AActor> OneSoloing) = 0;

	virtual void EndSolo() = 0;

	virtual FSoloStartEvent& GetOnSoloStartEvent() = 0;

	virtual FSoloEndEvent& GetOnSoloEndEvent() = 0;

	virtual FRoundStartEvent& GetOnRoundStartEvent() = 0;

	virtual FCombatStartEvent& GetOnCombatStartEvent() = 0;

	virtual FFightStartedEvent& GetOnFightStartedEvent() = 0;

	virtual FRoundEndEvent& GetOnRoundEndEvent() = 0;

	virtual FMatchEndEvent& GetOnMatchEndEvent() = 0;

	virtual FBeatWindowCloseEvent& GetOnBeatWindowCloseEvent() = 0;

	virtual float GetTimeLeftInRound() = 0;

	virtual FPlayerBeatComboChangedEvent& GetOnPlayerBeatComboChangedEvent() = 0;

	/** Hook for grid positioning to be enforced. Called when movement ends. */
	virtual void AdjustLocation(AActor* ActorToAdjust) = 0;

	virtual UObject* GetMusicBox() = 0;

	/** Determines if the given accuracy is considered on the beat. */
	virtual bool IsOnBeat(float Accuracy) = 0;

	/** Whether or not we are in the middle of a round. */
	virtual bool IsInRound() = 0;

	/** Get time until the next round starts. */
	virtual float TimeToNextRound() = 0;

	/** Should be called when the given player hit the opponent on the beat. */
	virtual void PlayerHitOnBeat(APlayerController* Player) = 0;

	/** Should be called when the given player hits off beat or misses the opponent. */
	virtual void PlayerMissBeat(APlayerController* Player) = 0;
	
	/** Called for Clash between fighters, if A won ret 1, if B ret -1, if tie ret 0. */
	virtual int OnClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB) = 0;

	/** Determines if there was a clash between two the Fighters, returns false if either of the Fighter's are NULL. */
	virtual bool CheckClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB) = 0;

	/** Gives the game mode a scaled time based on the beat */
	virtual float GetScaledTime(float time)  = 0;

};
