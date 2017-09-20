// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "InputParser.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInputParser : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IInputParser
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the fighterstate so it can query if input is blocked and send requests to be passed on to the Fighter. */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) = 0;

	/** Saves a reference to the movestate so it can push input tokens. */
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) = 0;

	/** Receives notification from FighterState when control may be input again. Input buffer should send its request after receiving such a signal. */
	virtual void OnControlReturned() = 0;

	/** Receives controller input for the horizontal axis. */
	virtual void InputAxisHorizontal(float Amount) = 0;

	/** Receives controller input for the vertical axis. */
	virtual void InputAxisVertical(float Amount) = 0;

	/** Receives events relating to the D-pad left button. */
	virtual void InputActionDLeft(bool IsUp) = 0;

	/** Receives events relating to the D-pad right button. */
	virtual void InputActionDRight(bool IsUp) = 0;

	/** Receives events relating to the D-pad down button. */
	virtual void InputActionDDown(bool IsUp) = 0;

	/** Receives events relating to the D-pad up button. */
	virtual void InputActionDUp(bool IsUp) = 0;

	/** Receives events relating to the punch button. */
	virtual void InputActionPunch(bool IsUp) = 0;
	
	/** Receives events relating to the kick button. */
	virtual void InputActionKick(bool IsUp) = 0;
};
