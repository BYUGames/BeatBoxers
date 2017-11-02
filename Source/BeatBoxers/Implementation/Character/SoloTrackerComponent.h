// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "Interfaces/ISoloTracker.h"
#include "Interfaces/IFighterWorld.h"
#include "Interfaces/IFighter.h"
#include "Interfaces/IFretboardFeed.h"
#include "../BasicFretboardFeed.h"
#include "SoloTrackerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API USoloTrackerComponent : public UActorComponent, public ISoloTracker, public IFretboardFeed
{
	GENERATED_UCLASS_BODY()

protected:
	IFighterWorld *MyFighterWorld;
	IFighter *MyFighter;
	UBasicFretboardFeed *MyFretboardFeed;

public:
	UFUNCTION(BlueprintCallable)
	UBasicFretboardFeed* GetFretboardFeed();

	// Called when the game starts
	virtual void BeginPlay() override;

	/** ISoloTracker implementation */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) override;
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) override;
	virtual void BeginSolo(FSoloParameters SoloParameters) override;
	virtual void EndSolo() override;
	/** End ISoloTracker implementation */

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

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
