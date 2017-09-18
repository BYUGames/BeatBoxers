// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "FighterState.h"
#include "Fighter.h"
#include "FighterWorld.h"
#include "Moveset.h"
#include "InputParser.h"
#include "FighterStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UFighterStateComponent : public UActorComponent, public IFighterState
{
	GENERATED_UCLASS_BODY()

protected:
	IFighterWorld *MyFighterWorld;
	IFighter *MyFighter;
	IMoveset *MyMoveset;
	IInputParser *MyInputParser;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	/** IFighterState implementation */
	virtual void RegisterFighterWorld(class UObject* FighterWorld) override;
	virtual void RegisterFighter(class UObject* Fighter) override;
	virtual void RegisterMoveset(class UObject* Moveset) override;
	virtual void RegisterInputParser(class UObject* InputParser) override;
	virtual bool IsInputBlocked() const override;
	virtual bool IsBlocking() const override;
	virtual void StartMoveWindow(FMoveWindow& Window, bool IsLastInSequence) override;
	virtual void StartStun(float Duration) override;
	virtual void SetMoveDirection(float Direction) override;
	virtual void SetWantsToCrouch(bool WantsToCrouch) override;
	virtual void Jump() override;
	/** End IFighterState implmementation */

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
