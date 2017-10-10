// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "ISoloTracker.h"
#include "IFretboardFeed.h"
#include "SoloTrackerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API USoloTrackerComponent : public UActorComponent, public ISoloTracker, public IFretboardFeed
{
	GENERATED_UCLASS_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FNewNoteEvent NewNoteEvent;

public:	
	/** ISoloTracker implementation */
	virtual void BeginSolo(FSoloParameters SoloParameters) override;
	virtual void EndSolo() override;
	/** End ISoloTracker implementation */

	/** IFretboardFeed implementation */
	virtual TArray<FNoteData> GetExistingNotes_Implementation() override;
	virtual FNewNoteEvent& GetOnNewNoteEvent() override;
	/** End IFretboardFeed implementation*/



	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
