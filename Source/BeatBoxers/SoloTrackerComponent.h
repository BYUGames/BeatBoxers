// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "SoloTracker.h"
#include "Moveset.h"
#include "SoloTrackerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API USoloTrackerComponent : public UActorComponent, public ISoloTracker
{
	GENERATED_UCLASS_BODY()

protected:
	IMoveset *MyMoveset;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	/** ISoloTracker implementation */
	virtual void RegisterMoveset(class UObject* Moveset) override;
	virtual void BeginSolo(FSoloParameters SoloParameters) override;
	virtual void ReceiveInputToken(EInputToken Input) override;
	/** End ISoloTracker implementation */

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
