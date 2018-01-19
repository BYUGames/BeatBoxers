// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "IFighterPlayerState.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFighterPlayerState : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFighterPlayerState
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/** Returns the amount of "special" the player has. */
	virtual float GetSpecial() const = 0;
	
	/** Returns the amount of "special" the player has as a percent of their maximum. */
	virtual float GetSpecialAsPercent() const = 0;

	/** Adds an amount to the player's special. */
	virtual void AddSpecial(float Amount) = 0;

	/** Uses an amount of the player's special. Returns false and does not modify special if there is not that much available, otherwise subtracts special amount and returns true. */
	virtual bool UseSpecial(float Amount) = 0;

	/** Returns the number of moves hit on the beat in the current combo. */
	virtual int GetBeatCombo() const = 0;

	/** Sets and returns the number of moves hit on the beat in the current combo. */
	virtual int SetBeatCombo(int Combo) = 0;

	/** Decraments Amount from Health to a min of 0 */
	virtual void TakeDamage(float Amount) = 0;

	/** Returns Player Health*/
	virtual float GetHealth() = 0;

	/** Resets Player state to original values */
	virtual void ResetPlayerState() = 0;
};
