// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "Moveset.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMoveset : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IMoveset
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the fighterstate so it can push requests to start new windows. */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) = 0;

	/** Saves a reference to the solo tracker so it can push requests to stat a solo. */
	virtual void RegisterSoloTracker(TWeakObjectPtr<UObject> SoloTracker) = 0;

	/** Call to execute whatever move, or move in current combo, comes next according to the given input. */
	virtual void ReceiveInputToken(EInputToken Token) = 0;

	/** Receives notifications from FighterState when its current window has finished and if it was interrupted or not. */
	virtual void OnWindowFinished(bool WasInterrupted) = 0;

	/** Receives a push from FighterState telling it to begin its solo. */
	virtual void BeginSolo() = 0;

	/** Receives a push from the SoloTracker if it correctly hit the note and with what input. */
	virtual void OnSoloCorrect(EInputToken Token) = 0;

	/** Receives a push from the SoloTracker if it missed or used the wrong input. */
	virtual void OnSoloIncorrect() = 0;
};
