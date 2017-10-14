// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxers.h"
#include "BeatBoxersStructs.h"
#include "IMusicBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeatEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMusicEndEvent);

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

	/** Called by the GameMode when the music box needs to start or restart the music. */
	virtual void StartMusic() = 0;

	/** Called by the GameMode if it needs to pause the music. */
	virtual void PauseMusic() = 0;

	/** Called by the GameMode if it needs to resume the music. */
	virtual void ResumeMusic() = 0;

	/** Called by the GameMode if it needs to stop the music. */
	virtual void StopMusic() = 0;

	/** Allows objects to query when the next beat will happen. Returns -beat interval if the beat is not currently... beating. */
	virtual float GetTimeToNextBeat() = 0;

	/** Allows objects to query when the Song will end. */
	virtual float GetTimeToSongEnd() = 0;

	/** Returns a reference to an event that occurs with every beat. */
	virtual FBeatEvent& GetOnBeatEvent() = 0;

	/** Returns a reference to an event that occurs at the end of the song. */
	virtual FMusicEndEvent& GetMusicEndEvent() = 0;
};