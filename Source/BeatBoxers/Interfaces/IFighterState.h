// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFretboard.h"
#include "Implementation/BasicFretboard.h"
#include "IFighterState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDDRToggleEvent, bool, IsInDDR);


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

	/** A check to see if the character is currently attempting to block. */
	virtual bool IsBlocking() const = 0;

	/** A check to see if the character is currently attempting to charge. */
	virtual bool IsCharging() const = 0;

	/** A check to see if the character is currently stunned or in knockdown. */
	virtual bool IsStunned() const = 0;

	/** A check to see if the character is currently stunned by blocking. */
	virtual bool IsBlockStunned() const = 0;

	/** A check to see if the character is currently in a knockdown state. */
	virtual bool IsKnockedDown() const = 0;

	/** A check to see if the character is in an invulnerable state. */
	virtual bool IsInvulnerable() const = 0;

	/** A check to see if character is currently in the middle of a move. */
	virtual bool IsMidMove() const = 0;

	/** Begins a move window. */
	virtual void StartMoveWindow(FMoveWindow& Window, float Accuracy) = 0;

	/** Interrupts any ongoing move windows and starts a stun. */
	virtual void StartStun(float Duration, bool WasBlocked) = 0;

	/** Interface to Fighter function, Fighter state can ignore function when it sees fit. */
	virtual void SetMoveDirection(float Direction) = 0;
	
	/** Interface to set direction of Vertical controller input*/
	virtual void SetVerticalDirection(float Direction) = 0;

	/** Interface to set direction of Horizontal controller input*/
	virtual void SetHorizontalDirection(float Direction) = 0;

	/** Interface to Fighter function, Fighter state can ignore function when it sees fit. */
	virtual void SetWantsToCrouch(bool WantsToCrouch) = 0;

	/** Interface to Fighter function, enable charging when it sees fit. */
	virtual void SetWantsToCharge(bool WantsToCharge) = 0;

	/** Applied movement to character. A new call will override this. */
	virtual void ApplyMovement(FMovement Movement) = 0;

	/** Interface to Fighter function, Fighter state can ignore function when it sees fit. */
	virtual void Jump(int direction) = 0;

	/** Interface to Fighter function, Fighter state can ignore function when it sees fit. */
	virtual void Block() = 0;

	/** Interface to Fighter function, Fighter state can ignore function when it sees fit. */
	virtual void StopBlock() = 0;

	/** Receives pushed notifications from Fighter when landing on the ground. */
	virtual void OnLand() = 0;

	/** Returns the amount of "special" the fighter has, acts as an interface to the playerstate. */
	virtual float GetSpecial() const = 0;

	/** Adds an amount to the fighter's special, acts as an interface to the playerstate. */
	virtual void AddSpecial(float Amount) = 0;

	/** Uses an amount of the fighter's special. Returns false and does not modify special if there is not that much available, otherwise subtracts special amount and returns true, acts as an interface to the playerstate. */
	virtual bool UseSpecial(float Amount) = 0;

	/** Sets the fighterstate knockdown. */
	virtual void Knockdown() = 0;

	/** Starts the recovery window to leave knockdown. */
	virtual void KnockdownRecovery(float Duration) = 0;

	/** An interface to the Fighter's GetStance function. */
	virtual EStance GetStance() const = 0;

	/** Get horizontal movement to expose to animation blueprint. */
	virtual float GetCurrentHorizontalMovement() const = 0;

	/** Get current vertical input. */
	virtual float GetCurrentVerticalDirection() const = 0;

	/** Get current horizontal input. */
	virtual float GetCurrentHorizontalDirection() const = 0;

	virtual void EndSolo() = 0;

	/** Returns true if the current window is ignoring collisions. */
	virtual bool IsIgnoringCollision() const = 0;

	/** Get accuracy of current move window, returns -1 if window is not active. */
	virtual float GetCurrentWindowAccuracy() const = 0;

	/** Returns true if in a move and the current move uses a hitbox, or false if not in a move or the move does not use a hitbox. */
	virtual bool DoesWindowUseHitbox() const = 0;

	/** Returns the hitbox to of the current window, if not in a window returns default FMoveHitbox */
	virtual FMoveHitbox GetHitbox() const = 0;

	/** Returns stage of current window. */
	virtual EWindowStage GetWindowStage() const = 0;

	/** Returns how many times the character has been hit during this knockdown. */
	virtual int GetTimesHitThisKnockdown() const = 0;

	/** Inform that it has been hit. */
	virtual void AddHit() = 0;

	/** Checks to see if player is in DDR state */
	virtual bool IsInDDR() = 0;

	/** Inform's the state of fighter being in DDR state, Will not trigger unless in stun OR Knockdown */
	virtual void StartDDR() = 0;

	/** Inform's the state of fighter leaving DDR state, Will not stop switch unless out of stun AND knockdown */
	virtual void EndDDR() = 0;

	virtual UBasicFretboard* GetFretboard() = 0;

	/** Returns true if teh move that the player is currently in is ment to be crouching. */
	virtual bool IsInCrouchMove() = 0;

	/** puts the player in a grabbed state */
	virtual bool Grabbed(float Duration) = 0;

	/** Releases the player from a grab */
	virtual void Released() = 0;

	/** Returns true if the player is currently grabbed */
	virtual bool IsGrabbed() const = 0;

	bool previousBeatHadAttack;
};
