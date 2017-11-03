// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFretboardFeed.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFretboardFeed : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IFretboardFeed
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Allows a new listener to get a collection of all the notes that are already existing in the feed. */
	virtual TArray<FFeedNoteData> GetExistingNotes() = 0;

	virtual void PauseFeed() = 0;
	virtual void ResumeFeed() = 0;
	virtual void ClearFeed() = 0;

	/** How early/long are notes added to the feed? */
	virtual float GetNoteLifetime() = 0;

	virtual void SetNoteLifetime(float NewLifetime) = 0;

	virtual void AddNote(FFeedNoteData FeedNote) = 0;

	virtual FNewFeedNoteEvent& GetOnNewFeedNoteEvent() = 0;
};
