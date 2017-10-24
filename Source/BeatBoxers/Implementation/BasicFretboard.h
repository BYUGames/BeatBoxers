// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxers.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "BeatBoxersStructs.h"
#include "Interfaces/IFretboard.h"
#include "Interfaces/IFretboardFeed.h"
#include "BasicFretboard.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class BEATBOXERS_API UBasicFretboard : public UObject, public IFretboard
{
	GENERATED_UCLASS_BODY()

protected:
	IFretboardFeed *MyFretboardFeed;
	int NextID;
	float Tolerance;
	FTimerManager *TimerManager;
	TMap<int, FNoteData> Notes;

public:
	/** Fires when a note is ended. 1 is best accuracy, 0 is worst, -1 if the note was not hit. */
	UPROPERTY(BlueprintAssignable)
	FNoteEndEvent NoteEndEvent;

	UPROPERTY(BlueprintAssignable)
	FNewNoteEvent NewNoteEvent;

	UPROPERTY(BlueprintAssignable)
	FNotesClearedEvent NotesClearedEvent;

	void SetTimerManager(FTimerManager& TimerManager) { UBasicFretboard::TimerManager = &TimerManager; }

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Listen"))
	void K2_SetTimerManage(UObject *Feed) { Listen(Feed); }

	/** [0..1], any percent greater than this counts as a hit. */
	void SetTolerance(float NewTolerance) { Tolerance = NewTolerance; }

	/** Implementation of IFretboard */
	virtual FFretboardInputResult ReceiveInputToken(EInputToken InputToken) override;
	virtual void Listen(TWeakObjectPtr<UObject> FretboardFeed) override;
	virtual void StopListening() override;
	virtual const TMap<int, FNoteData>& GetAllNotes() override { return Notes; }
	virtual void PauseBoard() override;
	virtual void ResumeBoard() override;
	virtual void ClearBoard() override;
	virtual FNewNoteEvent& GetOnNewNoteEvent() override { return NewNoteEvent; }
	virtual FNoteEndEvent& GetOnNoteEndEvent() override { return NoteEndEvent; }
	virtual FNotesClearedEvent& GetOnNotesClearedEvent() override { return NotesClearedEvent; }
	/** End implementation of IFretboard */

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Notes"))
	const TMap<int, FNoteData>& K2_GetNotes() { return GetAllNotes(); }

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Listen"))
	void K2_ListenToFeed(UObject *Feed) { Listen(Feed); }

	UFUNCTION(BlueprintCallable)
	float GetNotePercent(FNoteData NoteData);

	/** Returns the accuracy of hitting the note right now. -1 is a miss, 0 is worst possible accuracy, 1 is perfect accuracy. */
	UFUNCTION(BlueprintCallable)
	float GetNoteTimingAccuracy(FNoteData NoteData);

	UFUNCTION()
	void AddNote(FFeedNoteData FeedNote);

	UFUNCTION()
	void EndNote(int Index);
};
