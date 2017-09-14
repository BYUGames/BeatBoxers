// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "SoloTracker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USoloTracker : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATBOXERS_API ISoloTracker
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the Moveset so results of inputs can be pushed. */
	void RegisterMoveset(class UObject& Moveset);

	/** Starts a new solo based on the given parameters. */
	void BeginSolo(FSoloParameters SoloParameters);

	/** Receives input from the Moveset and pushes the results to the moveset. */
	void ReceiveInputToken(EInputToken Input);
};
