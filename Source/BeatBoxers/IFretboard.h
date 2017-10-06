// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "BeatBoxersStructs.h"
#include "IFretboard.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UFretboard : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFretboard
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Receives a token, returns if it was correct. */
	virtual FFretboardInputResult ReceiveInputToken(EInputToken InputToken) = 0;

	virtual void ListenToFeed(TWeakObjectPtr<UObject> FretboardFeed) = 0;

	virtual void StopListeningToFeed() = 0;
};
