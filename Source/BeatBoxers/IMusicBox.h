// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxers.h"
#include "BeatBoxersStructs.h"
#include "IMusicBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeatEvent);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UMusicBox : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * Anything that implements IMusicBox should also implement IFretboadFeed 
 */
class BEATBOXERS_API IMusicBox
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FBeatEvent& GetOnBeatEvent() = 0;
};