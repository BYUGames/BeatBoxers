// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "IInputParser.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
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
	/** Saves a reference to the FighterWorld. */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) = 0;

	/** Saves a reference to the Fighter. */
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) = 0;

	/** Saves a reference to the fighterstate so it can query if input is blocked and send requests to be passed on to the Fighter. */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) = 0;

	/** Saves a reference to the movestate so it can push input tokens. */
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) = 0;
	
	/** Saves a reference to the music box so it can push requests to stat a solo. */
	virtual void RegisterMusicBox(TWeakObjectPtr<UObject> MusicBox) = 0;

	/** Receives notification from FighterState when control may be input again. Input buffer should send its request after receiving such a signal. */
	virtual void OnControlReturned() = 0;

	/** Receives controller input for the horizontal axis. */
	virtual void InputAxisHorizontal(float Amount) = 0;

	/** Receives controller input for the vertical axis. */
	virtual void InputAxisVertical(float Amount) = 0;

	/** Receives events relating to the D-pad left button. */
	virtual void InputActionLeft(bool IsUp) = 0;

	/** Receives events relating to the D-pad right button. */
	virtual void InputActionRight(bool IsUp) = 0;

	/** Receives events relating to the D-pad down button. */
	virtual void InputActionDown(bool IsUp) = 0;

	/** Receives events relating to the D-pad up button. */
	virtual void InputActionUp(bool IsUp) = 0;

	/** Receives events relating to the light attack button. */
	virtual void InputActionLight(bool IsUp) = 0;

	/** Receives events relating to the medium attack button. */
	virtual void InputActionMedium(bool IsUp) = 0;
	
	/** Receives events relating to the heavy attack button. */
	virtual void InputActionHeavy(bool IsUp) = 0;

	/** Receives events relating to the Jump button. */
	virtual void InputActionJump(bool IsUp) = 0;

	/** Recives events related to the dash left button */
	virtual void InputActionDashLeft(bool IsUp) = 0;

	/** Recives events related to the dash right button */
	virtual void InputActionDashRight(bool IsUp) = 0;
};
