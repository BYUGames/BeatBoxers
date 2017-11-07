// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "Interfaces/IMoveset.h"
#include "Interfaces/IFighterWorld.h"
#include "Interfaces/IFighter.h"
#include "Interfaces/IFighterState.h"
#include "Interfaces/IInputParser.h"
#include "Interfaces/ISoloTracker.h"
#include "Interfaces/IMusicBox.h"
#include "../BasicFretboard.h"
#include "MovesetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UMovesetComponent : public UActorComponent, public IMoveset
{
	GENERATED_UCLASS_BODY()

private:
	FString cs; //Context String
	FDataTableRowHandle DefaultState;
	int CurrentWindowInState;

	// Don't call this. It does no checking.
	void SetState(FDataTableRowHandle State);

protected:
	IFighterWorld *MyFighterWorld;
	IFighter *MyFighter;
	IFighterState *MyFighterState;
	IInputParser *MyInputParser;
	ISoloTracker *MySoloTracker;
	IMusicBox *MyMusicBox;

	float InputAxis;

	EInputToken BufferToken;
	float BufferAccuracy;
	float MoveAccuracy;

	UPROPERTY()
	UBasicFretboard *BGFretboard;

	UPROPERTY()
	UBasicFretboard *SoloFretboard;

	FDataTableRowHandle CurrentState;
	FTimerHandle TimerHandle_PostWait;

	// Called when the game starts
	virtual void BeginPlay() override;

	/** Called when MaxDuration has been reached after finishing a move. */
	void OnPostWaitExpired();

	/** Goes to the default state. */
	void GotoDefaultState();

	/** Sets the current state to a new instance of the given state. */
	void GotoState(FDataTableRowHandle NewState);

	/** Starts the next window in the current move. */
	void StartNextWindow();

	void BindMusicBox(IMusicBox *MusicBox);
	void UnbindMusicBox(IMusicBox *MusicBox);

	UFUNCTION()
	void OnBeat();

	void ProcessInputToken(EInputToken Token, float Accuracy);

public:	
	UFUNCTION(BlueprintCallable)
	UBasicFretboard* GetBGFretboard();

	UFUNCTION(BlueprintCallable)
	UBasicFretboard* GetSoloFretboard();

	/** IMoveset implementation */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) override;
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) override;
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) override;
	virtual void RegisterInputParser(TWeakObjectPtr<UObject> InputParser) override;
	virtual void RegisterSoloTracker(TWeakObjectPtr<UObject> SoloTracker) override;
	virtual void RegisterMusicBox(TWeakObjectPtr<UObject> MusicBox) override;
	virtual void ReceiveInputToken(EInputToken Token) override;
	virtual void OnWindowFinished(EWindowEnd WindowEnd) override;
	virtual void OnSoloStart() override;
	virtual void UpdateInputAxis(float Axis) override { InputAxis = Axis; }
	/** End IMovesetImplementation */
};
