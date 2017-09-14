// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Fighter.h"
#include "FighterWorld.h"
#include "FighterState.h"
#include "Moveset.h"
#include "InputParser.h"
#include "SoloTracker.h"
#include "FighterStateComponent.h"
#include "MovesetComponent.h"
#include "InputParserComponent.h"
#include "SoloTrackerComponent.h"
#include "FighterCharacter.generated.h"

UCLASS()
class BEATBOXERS_API AFighterCharacter : public ACharacter, public IFighter
{
	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this character's properties
	AFighterCharacter();

	UFighterStateComponent* FighterState;
	UMovesetComponent* Moveset;
	UInputParserComponent* InputParser;
	USoloTrackerComponent* SoloTracker;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
