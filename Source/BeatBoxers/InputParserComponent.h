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
class UInputParserComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UInputParserComponent : public UActorComponent, public IInputParser
{
	GENERATED_UCLASS_BODY()

protected:
	class FInputParserState
	{
	protected:
		UInputParserComponent *Parent;

	public:
		FInputParserState(UInputParserComponent *Parent);
		void ChangeState(TSharedPtr<FInputParserState> NewState);

		virtual bool IsComplex();
		virtual void InputActionLeft();
		virtual void InputActionRight();
		virtual void InputActionDown();
		virtual void InputActionUp();
		virtual void InputActionPunch();
		virtual void InputActionKick();
	};

	class FDefaultState : public FInputParserState
	{
	public:
		FDefaultState(UInputParserComponent *Parent) : FInputParserState(Parent) {}

		virtual void InputActionLeft() override;
		virtual void InputActionRight() override;
		virtual void InputActionPunch() override;
		virtual void InputActionKick() override;
	};

	class FPreLeftDashState : public FDefaultState
	{
	public:
		FPreLeftDashState(UInputParserComponent *Parent) : FDefaultState(Parent) {}

		virtual bool IsComplex() override;
		virtual void InputActionLeft() override;
		virtual void InputActionPunch() override;
		virtual void InputActionKick() override;
	};

	class FPreRightDashState : public FDefaultState
	{
	public:
		FPreRightDashState(UInputParserComponent *Parent) : FDefaultState(Parent) {}

		virtual bool IsComplex() override;
		virtual void InputActionRight() override;
		virtual void InputActionPunch() override;
		virtual void InputActionKick() override;
	};

	// Amount of time the parser will hold the last token waiting for control to be returned before discarding it.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InputBufferLength;

	// Amount of time before a combo will not be registered as such.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ComplexInputWindow;

	IFighterState *MyFighterState;
	IMoveset *MyMoveset;
	FTimerHandle TimerHandle_InputBuffer;
	EInputToken InputBuffer;
	FTimerHandle TimerHandle_Combo;

	TSharedPtr<FInputParserState> CurrentState;

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

