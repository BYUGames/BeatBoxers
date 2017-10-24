// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IMusicBox.h"
#include "Interfaces/IFretboardFeed.h"
#include "BasicFretboardFeed.h"
#include "BasicMusicBox.generated.h"

UCLASS(BlueprintType)
class BEATBOXERS_API ABasicMusicBox : public AActor, public IMusicBox, public IFretboardFeed
{
	GENERATED_UCLASS_BODY()

protected:
	UBasicFretboardFeed *MyFretboardFeed;

public:
	UPROPERTY(BlueprintAssignable)
	FBeatEvent BeatEvent;

	UPROPERTY(BlueprintAssignable)
	FMusicEndEvent MusicEndEvent;

	UFUNCTION(BlueprintCallable)
	UBasicFretboardFeed* GetMyFretboardFeed();

	/** Implementation of IMusicBox */
	virtual int StartMusic() override;
	virtual int PauseMusic() override;
	virtual int ResumeMusic() override;
	virtual int StopMusic() override;
	virtual float GetTimeToNextBeat() override;
	virtual float GetTimeToSongEnd() override;
	virtual FBeatEvent& GetOnBeatEvent() override;
	virtual FMusicEndEvent& GetMusicEndEvent() override;
	/** End Implementation of IMusicBox */

	/** Implmementation of IFretboardFeed */
	virtual TArray<FFeedNoteData> GetExistingNotes() override { return GetMyFretboardFeed()->GetExistingNotes(); }
	virtual void PauseFeed() override { GetMyFretboardFeed()->PauseFeed(); }
	virtual void ResumeFeed() override { GetMyFretboardFeed()->ResumeFeed(); }
	virtual void ClearFeed() override { GetMyFretboardFeed()->ClearFeed(); }
	virtual float GetNoteLifetime() override { return GetMyFretboardFeed()->GetNoteLifetime(); }
	virtual void AddNote(FFeedNoteData FeedNote) override { GetMyFretboardFeed()->AddNote(FeedNote); }
	virtual FNewFeedNoteEvent& GetOnNewFeedNoteEvent() override { return GetMyFretboardFeed()->GetOnNewFeedNoteEvent(); }
	/** End Implementation of IFretboardFeed*/

	UFUNCTION(BlueprintCallable)
	void DropBeat();

	UFUNCTION(BlueprintCallable)
	void EndMusic();

	UFUNCTION(BlueprintCallable)
	void OnNewNote(FFeedNoteData NoteData);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintPure, meta = (displayname = "Get Time To Song End"))
	float K2_GetTimeToSongEnd() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintPure, meta=(displayname = "Get Time To Next Beat"))
	float K2_GetTimeToNextBeat() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta=(displayname = "Start Music"))
	int K2_StartMusic();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta = (displayname = "Resume Music"))
	int K2_ResumeMusic();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta = (displayname = "Pause Music"))
	int K2_PauseMusic();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta = (displayname = "Stop Music"))
	int K2_StopMusic();
};
