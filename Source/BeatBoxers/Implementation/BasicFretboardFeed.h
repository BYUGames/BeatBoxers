// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxers.h"
#include "BeatBoxersStructs.h"
#include "Interfaces/IFretboardFeed.h"
#include "UObject/NoExportTypes.h"
#include "BasicFretboardFeed.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BEATBOXERS_API UBasicFretboardFeed : public UObject, public IFretboardFeed
{
	GENERATED_UCLASS_BODY()
	
protected:
	TMap<int, FNoteData> Notes;
	int NextID;
	FTimerManager *TimerManager;
	float NoteDuration;
	
public:
	UPROPERTY(BlueprintAssignable)
	FNewFeedNoteEvent NewFeedNoteEvent;

	void SetTimerManager(FTimerManager& TimerManager) { UBasicFretboardFeed::TimerManager = &TimerManager; }

	void SetDuration(float NewDuration) { NoteDuration = NewDuration; }

	/** Begin IFretboardFeed Implementation */
	virtual TArray<FFeedNoteData> GetExistingNotes() override;
	virtual void PauseFeed() override;
	virtual void ResumeFeed() override;
	virtual void ClearFeed() override;
	virtual float GetNoteLifetime() override { return NoteDuration; }
	virtual void AddNote(FFeedNoteData FeedNote) override;
	virtual FNewFeedNoteEvent& GetOnNewFeedNoteEvent() override { return NewFeedNoteEvent; }
	/** End IFretboardFeed Implementation */

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Add Note"))
	void K2_AddNote(FFeedNoteData NewNote) { AddNote(NewNote); }

	void EndNote(int NoteID);
};
