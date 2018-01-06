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

	virtual void OnInputReceived() = 0;

	virtual FDataTableRowHandle GetDefaultMoveState() = 0;

	virtual void HitOnBeatLogic() = 0;

	virtual void InputOnBeatLogic() = 0;
};
