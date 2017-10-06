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

public:	
	/** Implementation of IMusicBox */
	virtual FBeatEvent& GetOnBeatEvent() override;
	/** End Implementation of IMusicBox */

	/** Implmementation of IFretboardFeed */
	virtual TArray<FNoteData> GetExistingNotes_Implementation() override;

	virtual FNewNoteEvent& GetOnNewNoteEvent() override;
	/** End Implementation of IFretboardFeed */

	UPROPERTY(BlueprintAssignable)
	FNewNoteEvent NewNoteEvent;

	UPROPERTY(BlueprintAssignable)
	FBeatEvent BeatEvent;
};
