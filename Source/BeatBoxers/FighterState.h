// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "FighterState.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFighterState : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFighterState
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the Fighter so it can act as an interface to it. */
	void RegisterFighter(class UObject& Fighter);

	/** Saves a reference to the Moveset so it can push notifications about when a window ends. */
	void RegisterMoveset(class UObject& Moveset);

	/** Saves a reference to the InputParser so it can notify it when control is returned. */
	void RegisterInputParser(class UObject& InputParser);

	/** A check if input is being accepted right now or should be buffered. */
	bool IsInputBlocked();

	/** A check to see if the character is currently attempting to block. Relies on opponent direction received from Fighter. */
	bool IsBlocking();

	/** Begins a move window. */
	void StartMoveWindow(FMoveWindow& Window, bool IsLastInSequence);

	/** Interrupts any ongoing move windows and starts a stun. */
	void StartStun(float Duration);

	/** Interface to Fighter function, blocks when it sees fit. */
	void SetMoveDirection(float Direction);

	/** Interface to Fighter function, blocks when it sees fit. */
	void SetWantsToCrouch(bool WantsToCrouch);

	/** Interface to Fighter function, blocks when it sees fit. */
	void Jump();
};
