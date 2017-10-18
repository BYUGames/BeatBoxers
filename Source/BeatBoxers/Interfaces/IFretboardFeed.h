// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "IFretboardFeed.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewNoteEvent, FNoteData, NoteData);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
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
	UFUNCTION(BlueprintNativeEvent)
	TArray<FNoteData> GetExistingNotes();

	virtual FNewNoteEvent& GetOnNewNoteEvent() = 0;
};
