// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IMoveset.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
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
	/** Saves a reference to the FighterWorld. */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) = 0;

	/** Saves a reference to the Fighter. */
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) = 0;

	/** Saves a reference to the fighterstate so it can push requests to start new windows. */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) = 0;

	/** Saves a reference to the input parser so it can push notifications of when the current move has finished. */
	virtual void RegisterInputParser(TWeakObjectPtr<UObject> InputParser) = 0;

	/** Saves a reference to the solo tracker so it can push requests to stat a solo. */
	virtual void RegisterSoloTracker(TWeakObjectPtr<UObject> SoloTracker) = 0;

	/** Saves a reference to the music box so it can push requests to stat a solo. */
	virtual void RegisterMusicBox(TWeakObjectPtr<UObject> MusicBox) = 0;

	/** Call to execute whatever move, or move in current combo, comes next according to the given input. */
	virtual void ReceiveInputToken(FBufferInputToken Token) = 0;

	/** Receives notifications from FighterState when its current window has finished and if it was interrupted or not. */
	virtual void OnWindowFinished(EWindowEnd WindowEnd) = 0;

	/** Function call to alert the moveset that it has entered it's solo. */
	virtual void OnSoloStart() = 0;
};
