// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "BeatBoxersStructs.h"
#include "NoteKeeper.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNoteEndEvent, int, NoteID, float, Accuracy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewNoteKeeperNoteEvent, int, NoteID, FNoteData, NoteData);

/**
 * 
 */
UCLASS(BlueprintType)
class BEATBOXERS_API UNoteKeeper : public UObject
{
	GENERATED_UCLASS_BODY()

protected:
	int NextID;
	float Tolerance;
	FTimerManager *TimerManager;
	TMap<int, FNoteData> Notes;
	
public:
	/** Fires when a note is ended. If accuracy >= 0 the note was hit, -1 if missed. */
	UPROPERTY(BlueprintAssignable)
	FNoteEndEvent NoteEndEvent;

	UPROPERTY(BlueprintAssignable)
	FNewNoteKeeperNoteEvent NewNoteKeeperNoteEvent;

	void SetTimerManager(FTimerManager& TimerManager) { UNoteKeeper::TimerManager = &TimerManager; }

	void SetTolerance(float NewTolerance) { Tolerance = NewTolerance; }

	UFUNCTION(BlueprintCallable)
	const TMap<int, FNoteData>& GetNotes() const { return Notes; }

	/** Attempts to hit any notes of given type. Returns accuracy, or -1 if no notes were hit. */
	float HitNoteType(ENoteType Type);

	void AddNote(FNoteData NewNote);

	UFUNCTION(BlueprintCallable)
	float GetNoteTimingAccuracy(FNoteData& NoteData) const;

	UFUNCTION()
	void EndNote(int Index);
};
