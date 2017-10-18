// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "ISoloTracker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USoloTracker : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Anything that implements ISoloTracker should also implement IFretboardFeed
 */
class BEATBOXERS_API ISoloTracker
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Starts a new solo based on the given parameters. */
	virtual void BeginSolo(FSoloParameters SoloParameters) = 0;

	/** Stop whatever solo is currently active. */
	virtual void EndSolo() = 0;
};
