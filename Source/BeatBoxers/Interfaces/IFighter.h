 // copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFighter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartJumpEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLandEvent);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UFighter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFighter
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the fighterstate so it can act as a public interface for it. */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) = 0;

	/** Saves a reference to the opponent. */
	virtual void RegisterOpponent(TWeakObjectPtr<AActor> Opponent) = 0;

	/** Returns the direction to the opponent. -1 is left, 1 is right. Provided so FighterState can tell if it is attempting to block. */
	virtual float GetOpponentDirection() const = 0;

	/** Moves the character... somehow... maybe this should be handled by the FighterState, in which case this would be an interface for it. */
	virtual void ApplyMovement(FMovement Movement) = 0;

	/** Interface for FighterState to see if character is attempting to block. */
	virtual bool IsBlocking() const = 0;

	/** Whether the fighter is currently invulnerable. */
	virtual bool IsInvulnerable() const = 0;

	/** Returns the current stance. */
	virtual EStance GetStance() const = 0;

	/** Set the fighter's facing. */
	virtual void SetFacing(float Sign) = 0;

	/** Get the fighter's facing. */
	virtual float GetFacing() const = 0;

	/** Get the figher's controller. */
	virtual TWeakObjectPtr<AController> GetFighterController() const = 0;

	/** Recieves calls from FighterState, gating logic should have been done there. */
	virtual void SetWantsToCrouch(bool WantsToCrouch) = 0;

	/** Recieves calls from FighterState, gating logic should have been done there. */
	virtual void SetMoveDirection(float Direction) = 0;

	/** Recieves calls from FighterState, gating logic should have been done there. */
	virtual void Jump() = 0;

	virtual FStartJumpEvent& GetOnStartJumpEvent() = 0;

	virtual FLandEvent& GetOnLandEvent() = 0;

	/** Used to expose event to blueprints, fires when the moveset component recieves input from the input parser. */
	virtual void OnInputReceived() = 0;

	virtual FDataTableRowHandle GetDefaultMoveState() = 0;

	/** Called when an on-beat attack hits the opponent. */
	virtual void HitOnBeatLogic() = 0;

	/** Called when an attack input is on the beat. */
	virtual void InputOnBeatLogic() = 0;

	/** Called when an attack is off beat or misses the opponent. */
	virtual void MissBeat() = 0;

	/** Returns true if IFighter is in jumping process */
	virtual bool IsJumping() = 0;

	/** Returns if we're "dead" */
	virtual bool IsDead() = 0;

	/** Knocksdown the fighter. */
	virtual void Knockdown() = 0;

	/** Returns the music balance for this fighter. */
	virtual FMusicBalanceParams GetMusicBalance() = 0;

	/** If DoesCollide is true then the player will collide with other players, if false it will ignore them. */
	virtual void SetFighterCollisions(bool DoesCollide) = 0;

	/** Interrupts any ongoing move windows and starts a stun. */
	virtual void StartStun(float Duration, bool WasBlocked) = 0;
};
