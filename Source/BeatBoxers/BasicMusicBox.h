// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IMusicBox.h"
#include "IFretboardFeed.h"
#include "BasicMusicBox.generated.h"

UCLASS(BlueprintType)
class BEATBOXERS_API ABasicMusicBox : public AActor, public IMusicBox, public IFretboardFeed
{
	GENERATED_UCLASS_BODY()
	
protected:
	//TODO: replace this with actual music stuff
	

public:	

	UPROPERTY(BlueprintAssignable)
	FNewNoteEvent NewNoteEvent;

	UPROPERTY(BlueprintAssignable)
	FBeatEvent BeatEvent;

	UPROPERTY(BlueprintAssignable)
	FMusicEndEvent MusicEndEvent;


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
	virtual TArray<FNoteData> GetExistingNotes_Implementation() override;

	virtual FNewNoteEvent& GetOnNewNoteEvent() override;
	/** End Implementation of IFretboardFeed */

	void ABasicMusicBox::EndMusic();
	void ABasicMusicBox::OnNewNote(FNoteData data);

	UFUNCTION(BlueprintCallable)
	void DropBeat();

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
