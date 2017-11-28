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

	virtual void Listen(TWeakObjectPtr<UObject> NoteFeed) = 0;
	virtual void StopListening() = 0;

	virtual void PauseBoard() = 0;
	virtual void ResumeBoard() = 0;
	virtual void ClearBoard() = 0;

	virtual const TMap<int, FNoteData>& GetAllNotes() = 0;

	/** Quickly returns the percent for a specified note. */
	virtual float GetNotePercent(FNoteData NoteData) = 0;

	virtual FNewNoteEvent& GetOnNewNoteEvent() = 0;
	virtual FNoteEndEvent& GetOnNoteEndEvent() = 0;
	virtual FNotesClearedEvent& GetOnNotesClearedEvent() = 0;
};
