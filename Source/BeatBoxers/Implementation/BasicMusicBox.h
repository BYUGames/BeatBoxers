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
	UPROPERTY()
	UBasicFretboardFeed *MyFretboardFeed;

public:
	UPROPERTY(BlueprintAssignable)
	FBeatEvent BeatEvent;

	UPROPERTY(BlueprintAssignable)
	FMusicEndEvent MusicEndEvent;

	UFUNCTION(BlueprintCallable)
	UBasicFretboardFeed* GetFretboardFeed();

	/** Implementation of IMusicBox */
	virtual int StartMusic(FName Songname, FMusicBalanceParams MusicBalance) override;
	virtual int ChangeBalance(FMusicBalanceParams MusicBalance) override;
	virtual int PauseMusic() override;
	virtual int ResumeMusic() override;
	virtual int StopMusic() override;
	virtual float GetTimeToNextBeat() const override;
	virtual float GetTimeBetweenBeats() const override;
	virtual float GetBeatAccuracy() const override;
	virtual float GetTimeToSongEnd() const override;
	virtual FBeatEvent& GetOnBeatEvent() override;
	virtual FMusicEndEvent& GetMusicEndEvent() override;
	/** End Implementation of IMusicBox */

	/** Implmementation of IFretboardFeed */
	virtual TArray<FFeedNoteData> GetExistingNotes() override { return GetFretboardFeed()->GetExistingNotes(); }
	virtual void PauseFeed() override { GetFretboardFeed()->PauseFeed(); }
	virtual void ResumeFeed() override { GetFretboardFeed()->ResumeFeed(); }
	virtual void ClearFeed() override { GetFretboardFeed()->ClearFeed(); }
	virtual float GetNoteLifetime() override { return GetFretboardFeed()->GetNoteLifetime(); }
	virtual void SetNoteLifetime(float NewDuration) override { return GetFretboardFeed()->SetNoteLifetime(NewDuration); }
	virtual void AddNote(FFeedNoteData FeedNote) override { GetFretboardFeed()->AddNote(FeedNote); }
	virtual FNewFeedNoteEvent& GetOnNewFeedNoteEvent() override { return GetFretboardFeed()->GetOnNewFeedNoteEvent(); }
	/** End Implementation of IFretboardFeed*/

	UFUNCTION(BlueprintCallable)
	void DropBeat();

	UFUNCTION(BlueprintCallable)
	void EndMusic();

	UFUNCTION(BlueprintCallable)
	void OnNewNote(FFeedNoteData NoteData) { AddNote(NoteData); }

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintPure, meta = (displayname = "Get Time To Song End"))
	float K2_GetTimeToSongEnd() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintPure, meta=(displayname = "Get Time To Next Beat"))
	float K2_GetTimeToNextBeat() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintPure, meta=(displayname = "Get Time Between Beats"))
	float K2_GetTimeBetweenBeats() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintPure, meta=(displayname = "Get Beat Accuracy"))
	float K2_GetBeatAccuracy() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta=(displayname = "Start Music"))
	int K2_StartMusic(FName SongName, FMusicBalanceParams MusicBalance);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta=(displayname = "Change Music Balance"))
	int K2_ChangeBalance(FMusicBalanceParams MusicBalance);

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta = (displayname = "Resume Music"))
	int K2_ResumeMusic();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta = (displayname = "Pause Music"))
	int K2_PauseMusic();

	UFUNCTION(BlueprintNativeEvent, Category = "Event", BlueprintCallable, meta = (displayname = "Stop Music"))
	int K2_StopMusic();
};
