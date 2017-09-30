// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "Moveset.h"
#include "FighterState.h"
#include "InputParser.h"
#include "SoloTracker.h"
#include "MovesetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UMovesetComponent : public UActorComponent, public IMoveset
{
	GENERATED_UCLASS_BODY()

private:
	TSubclassOf<AMoveState> DefaultState;

	// Don't call this. It does no checking.
	void SetState(TSubclassOf<AMoveState> State);

protected:
	IFighterState *MyFighterState;
	IInputParser *MyInputParser;
	ISoloTracker *MySoloTracker;

	AMoveState* CurrentState;
	FTimerHandle TimerHandle_PostWait;

	// Called when the game starts
	virtual void BeginPlay() override;

	/** Gets the current state or resets it to default if it can't be found. */
	AMoveState* GetCurrentState();

	/** Called when MaxDuration has been reached after finishing a move. */
	void OnPostWaitExpired();

	/** Goes to the default state. */
	void GotoDefaultState();

	/** Sets the current state to a new instance of the given state. */
	void GotoState(TSubclassOf<AMoveState> NewState);

	/** Starts the next window in the current move. */
	void StartNextWindow();

public:	

	/** IMoveset implementation */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) override;
	virtual void RegisterInputParser(TWeakObjectPtr<UObject> InputParser) override;
	virtual void RegisterSoloTracker(TWeakObjectPtr<UObject> SoloTracker) override;
	virtual void ReceiveInputToken(EInputToken Token) override;
	virtual void OnWindowFinished(EWindowEnd WindowEnd) override;
	virtual void BeginSolo() override;
	virtual void OnSoloCorrect(EInputToken Token) override;
	virtual void OnSoloIncorrect() override;
	/** End IMovesetImplementation */
};
