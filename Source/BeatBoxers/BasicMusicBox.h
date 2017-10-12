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
	//TODO replace this with actual music stuff
	FTimerHandle TimerHandle_Beat;

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TimeBetweenBeats;

	UPROPERTY(BlueprintAssignable)
	FNewNoteEvent NewNoteEvent;

	UPROPERTY(BlueprintAssignable)
	FBeatEvent BeatEvent;

	/** Implementation of IMusicBox */
	virtual void StartMusic() override;
	virtual void PauseMusic() override;
	virtual void ResumeMusic() override;
	virtual void StopMusic() override;
	virtual float GetTimeToNextBeat() override;
	virtual FBeatEvent& GetOnBeatEvent() override;
	/** End Implementation of IMusicBox */

	/** Implmementation of IFretboardFeed */
	virtual TArray<FNoteData> GetExistingNotes_Implementation() override;

	virtual FNewNoteEvent& GetOnNewNoteEvent() override;
	/** End Implementation of IFretboardFeed */

	UFUNCTION(BlueprintCallable)
	void DropBeat();
};
