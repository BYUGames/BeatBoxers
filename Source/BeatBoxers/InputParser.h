// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputParser.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInputParser : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATBOXERS_API IInputParser
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Saves a reference to the fighterstate so it can query if input is blocked and send requests to be passed on to the Fighter. */
	void RegisterFighterState(class UObject& FighterState);

	/** Saves a reference to the movestate so it can push input tokens. */
	void RegisterMoveset(class UObject& Moveset);

	/** Receives notification from FighterState when control may be input again. Input buffer should send its request after receiving such a signal. */
	void OnControlReturned();

	/** Receives controller input for the horizontal axis. */
	void InputAxisHorizontal(float Amount);

	/** Receives controller input for the vertical axis. */
	void InputAxisVertical(float Amount);

	/** Receives events relating to the D-pad left button. */
	void InputActionDLeft(bool IsUp);

	/** Receives events relating to the D-pad right button. */
	void InputActionDRight(bool IsUp);

	/** Receives events relating to the D-pad down button. */
	void InputActionDDown(bool IsUp);

	/** Receives events relating to the D-pad up button. */
	void InputActionDUp(bool IsUp);

	/** Receives events relating to the punch button. */
	void InputActionPunch(bool IsUp);
	
	/** Receives events relating to the kick button. */
	void InputActionKick(bool IsUp);
};
