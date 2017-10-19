// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFighterState.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UFighterState : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFighterState
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the FighterWorld so it can push calls to trace etc. */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) = 0;

	/** Saves a reference to the Fighter so it can act as an interface to it. */
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) = 0;

	/** Saves a reference to the Moveset so it can push notifications about when a window ends. */
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) = 0;

	/** Saves a reference to the InputParser so it can notify it when control is returned after a stun. */
	virtual void RegisterInputParser(TWeakObjectPtr<UObject> InputParser) = 0;

	/** Saves a reference to the BBPlayerState so it can use its special. */
	virtual void RegisterFighterPlayerState(TWeakObjectPtr<UObject> FighterPlayerState) = 0;

	/** A check if input is being accepted right now or should be buffered. */
	virtual bool IsInputBlocked() const = 0;

	/** A check to see if the character is currently attempting to block. Relies on opponent direction received from Fighter. */
	virtual bool IsBlocking() const = 0;

	/** A check to see if character is currently stunned. */
	virtual bool IsStunned() const = 0;

	/** A check to see if character is currently in the middle of a move. */
	virtual bool IsMidMove() const = 0;

	/** Begins a move window. */
	virtual void StartMoveWindow(FMoveWindow& Window) = 0;

	/** Interrupts any ongoing move windows and starts a stun. */
	virtual void StartStun(float Duration, bool WasBlocked) = 0;

	/** Interface to Fighter function, blocks when it sees fit. */
	virtual void SetMoveDirection(float Direction) = 0;

	/** Interface to Fighter function, blocks when it sees fit. */
	virtual void SetWantsToCrouch(bool WantsToCrouch) = 0;

	/** Applied movement to character. A new call will override this. */
	virtual void ApplyMovement(FMovement Movement) = 0;

	/** Interface to Fighter function, blocks when it sees fit. */
	virtual void Jump() = 0;

	/** Receives pushed notifications from Fighter when landing on the ground. */
	virtual void OnLand() = 0;

	/** Returns the amount of "special" the fighter has, acts as an interface to the playerstate. */
	virtual float GetSpecial() const = 0;

	/** Adds an amount to the fighter's special, acts as an interface to the playerstate. */
	virtual void AddSpecial(float Amount) = 0;

	/** Uses an amount of the fighter's special. Returns false and does not modify special if there is not that much available, otherwise subtracts special amount and returns true, acts as an interface to the playerstate. */
	virtual bool UseSpecial(float Amount) = 0;

	/** An interface to the Fighter's GetStance function. */
	virtual EStance GetStance() const = 0;

	/** Interface to get DefaultMoveState from the Fighter. */
	virtual TSubclassOf<AMoveState> GetDefaultMoveState() = 0;

	/** Get horizontal movement to expose to animation blueprint. */
	virtual float GetCurrentHorizontalMovement() const = 0;

	virtual void EndSolo() = 0;
};
