// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "Fighter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFighter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFighter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the fighterstate so it can act as a public interface for it. */
	void RegisterFighterState(class UObject& FighterState);

	/** Saves a reference to the opponent. */
	void RegisterOpponent(class AActor& Opponent);

	/** Returns the direction to the opponent. -1 is left, 1 is right. Provided so FighterState can tell if it is attempting to block. */
	float GetOpponentDirection();

	/** Moves the character... somehow... maybe this should be handled by the FighterState, in which case this would be an interface for it. */
	void ApplyMovement(FMovement Delta);

	/** Interface for FighterState to see if character is attempting to block. */
	bool IsBlocking();

	/** Returns the current stance. */
	EStance GetStance();

	/** Recieves calls from FighterState, gating logic should have been done there. */
	void SetWantsToCrouch(bool WantsToCrouch);

	/** Recieves calls from FighterState, gating logic should have been done there. */
	void SetMoveDirection(float Direction);

	/** Recieves calls from FighterState, gating logic should have been done there. */
	void Jump();
};
