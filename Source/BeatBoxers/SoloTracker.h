// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "SoloTracker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USoloTracker : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API ISoloTracker
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the Moveset so results of inputs can be pushed. */
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) = 0;

	/** Starts a new solo based on the given parameters. */
	virtual void BeginSolo(FSoloParameters SoloParameters) = 0;

	/** Receives input from the Moveset and pushes the results to the moveset. */
	virtual void ReceiveInputToken(EInputToken Input) = 0;
};
