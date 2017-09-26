// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "InputParser.h"
#include "Moveset.h"
#include "FighterState.h"
#include "InputParserComponent.generated.h"

class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UInputParserComponent : public UActorComponent, public IInputParser
{
	GENERATED_UCLASS_BODY()

protected:
	// Amount of time the parser will hold the last token waiting for control to be returned before discarding it.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InputBufferLength;

	// Amount of time before a combo will not be registered as such.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ComplexInputWindow;

	// TODO replace this with a state machine.
	int LastDashInput;

	IFighterState *MyFighterState;
	IMoveset *MyMoveset;
	FTimerHandle TimerHandle_InputBuffer;
	EInputToken InputBuffer;
	FTimerHandle TimerHandle_Combo;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	void OnInputBufferTimer();
	void SetInputBuffer(EInputToken NewToken);

	void StartComboTimer();
	void OnComboTimer();

	void PushInputToken(EInputToken NewToken);

public:	
	/** IInputParser implementation */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) override;
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) override;
	virtual void OnControlReturned() override;
	virtual void InputAxisHorizontal(float Amount) override;
	virtual void InputAxisVertical(float Amount) override;
	virtual void InputActionLeft(bool IsUp) override;
	virtual void InputActionRight(bool IsUp) override;
	virtual void InputActionDown(bool IsUp) override;
	virtual void InputActionUp(bool IsUp) override;
	virtual void InputActionPunch(bool IsUp) override;
	virtual void InputActionKick(bool IsUp) override;
	/** End IInputParser implementation */

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
