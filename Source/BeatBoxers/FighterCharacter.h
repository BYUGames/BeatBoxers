// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BeatBoxers.h"
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
	// Do not change these to const members, it will not be enforced because of the getters.
	UFighterStateComponent *FighterState;
	UMovesetComponent *Moveset;
	UInputParserComponent *InputParser;
	USoloTrackerComponent *SoloTracker;

protected:
	IFighterState *MyFighterState;
	TWeakObjectPtr<AActor> MyOpponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	IFighter* GetSelfAsFighter();
	const IFighter* GetSelfAsFighter() const;
	IFighterState* GetFighterState();
	const IFighterState* GetFighterState() const;
	IMoveset* GetMoveset();
	const IMoveset* GetMoveset() const;
	IInputParser* GetInputParser();
	const IInputParser* GetInputParser() const;
	ISoloTracker* GetSoloTracker();
	const ISoloTracker* GetSoloTracker() const;
	IFighterWorld* GetFighterWorld();
	const IFighterWorld* GetFighterWorld() const;

public:	
	/** IFighter implementation */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> NewFighterState) override;
	virtual void RegisterOpponent(TWeakObjectPtr<AActor> Opponent) override;
	virtual float GetOpponentDirection() const override;
	virtual void ApplyMovement(FMovement Delta) override;
	virtual bool IsBlocking() const override;
	virtual EStance GetStance() const override;
	virtual TWeakObjectPtr<AController> GetFighterController() const override;
	virtual void SetWantsToCrouch(bool WantsToCrouch) override;
	virtual void SetMoveDirection(float Direction) override;
	virtual void Jump() override;
	/** End IFighter implementation */

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;
	
};
