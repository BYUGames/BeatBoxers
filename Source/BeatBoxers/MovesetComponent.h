// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "IMoveset.h"
#include "IFighterState.h"
#include "IInputParser.h"
#include "ISoloTracker.h"
#include "MovesetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UMovesetComponent : public UActorComponent, public IMoveset
{
	GENERATED_UCLASS_BODY()

private:
	TSubclassOf<AMoveState> DefaultStateClass;
	int CurrentWindowInState;

	// Don't call this. It does no checking.
	void SetState(TSubclassOf<AMoveState> State);

protected:
	IFighterState *MyFighterState;
	IInputParser *MyInputParser;
	ISoloTracker *MySoloTracker;

	TSubclassOf<AMoveState> CurrentStateClass;
	FTimerHandle TimerHandle_PostWait;

	// Called when the game starts
	virtual void BeginPlay() override;

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
	/** End IMovesetImplementation */
};
