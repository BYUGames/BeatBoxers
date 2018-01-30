// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "Interfaces/IInputParser.h"
#include "Interfaces/IFighterWorld.h"
#include "Interfaces/IFighter.h"
#include "Interfaces/IMoveset.h"
#include "Interfaces/IFighterState.h"
#include "Interfaces/IMusicBox.h"
#include "InputParserComponent.generated.h"


class UInputParserComponent;

UCLASS()
class UInputParserState : public UObject
{
	GENERATED_BODY()

public:
	void ChangeState(UInputParserComponent *Parser, TSubclassOf<UInputParserState> NewState);

	virtual bool IsComplex();
	virtual void InputActionLeft(UInputParserComponent *Parser);
	virtual void InputActionRight(UInputParserComponent *Parser);
	virtual void InputActionDown(UInputParserComponent *Parser);
	virtual void InputActionUp(UInputParserComponent *Parser);
	virtual void InputActionLight(UInputParserComponent *Parser);
	virtual void InputActionMedium(UInputParserComponent *Parser);
	virtual void InputActionHeavy(UInputParserComponent *Parser);
	virtual void InputActionSpecial1(UInputParserComponent *Parser);
	virtual void InputActionForwardLight(UInputParserComponent *Parser);
	virtual void InputActionBackLight(UInputParserComponent *Parser);
	virtual void InputActionDashForward(UInputParserComponent *Parser);
	virtual void InputActionDashBackwards(UInputParserComponent * Parser);
};

UCLASS()
class UInputParserDefaultState : public UInputParserState
{
	GENERATED_BODY()

public:
	virtual void InputActionLeft(UInputParserComponent *Parser) override;
	virtual void InputActionRight(UInputParserComponent *Parser) override;
	virtual void InputActionLight(UInputParserComponent *Parser) override;
	virtual void InputActionMedium(UInputParserComponent *Parser) override;
	virtual void InputActionHeavy(UInputParserComponent *Parser) override;
	virtual void InputActionSpecial1(UInputParserComponent *Parser) override;
	virtual void InputActionForwardLight(UInputParserComponent *Parser) override;
	virtual void InputActionBackLight(UInputParserComponent *Parser) override;
	virtual void InputActionDashForward(UInputParserComponent *Parser) override;
	virtual void InputActionDashBackwards(UInputParserComponent *Parser) override;
};

UCLASS()
class UPreLeftDashState : public UInputParserDefaultState
{
	GENERATED_BODY()

public:
	virtual bool IsComplex() override;
	virtual void InputActionLeft(UInputParserComponent *Parser) override;
	virtual void InputActionLight(UInputParserComponent *Parser) override;
	virtual void InputActionMedium(UInputParserComponent *Parser) override;
	virtual void InputActionHeavy(UInputParserComponent *Parser) override;
	virtual void InputActionSpecial1(UInputParserComponent *Parser) override;
};

UCLASS()
class UPreRightDashState : public UInputParserDefaultState
{
	GENERATED_BODY()

public:
	virtual bool IsComplex() override;
	virtual void InputActionRight(UInputParserComponent *Parser) override;
	virtual void InputActionLight(UInputParserComponent *Parser) override;
	virtual void InputActionMedium(UInputParserComponent *Parser) override;
	virtual void InputActionHeavy(UInputParserComponent *Parser) override;
	virtual void InputActionSpecial1(UInputParserComponent *Parser) override;
};

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

	IFighterWorld *MyFighterWorld;
	IFighter *MyFighter;
	IFighterState *MyFighterState;
	IMoveset *MyMoveset;
	FTimerHandle TimerHandle_InputBuffer;
	FBufferInputToken InputBuffer;
	FTimerHandle TimerHandle_Combo;
	IMusicBox *MyMusicBox;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	void OnInputBufferTimer();
	void SetInputBuffer(FBufferInputToken NewToken);

	void OnComboTimer();
	float calcAccuracy();
public:	
	TSubclassOf<UInputParserState> CurrentStateClass;

	/** IInputParser implementation */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) override;
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) override;
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) override;
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) override;
	virtual void RegisterMusicBox(TWeakObjectPtr<UObject> MusicBox) override;
	virtual void OnControlReturned() override;
	virtual void InputAxisHorizontal(float Amount) override;
	virtual void InputAxisVertical(float Amount) override;
	virtual void InputActionLeft(bool IsUp) override;
	virtual void InputActionRight(bool IsUp) override;
	virtual void InputActionDown(bool IsUp) override;
	virtual void InputActionUp(bool IsUp) override;
	virtual void InputActionLight(bool IsUp) override;
	virtual void InputActionMedium(bool IsUp) override;
	virtual void InputActionHeavy(bool IsUp) override;
	virtual void InputActionSpecial1(bool IsUp) override;
	virtual void InputActionJump(bool IsUp) override;
	virtual void InputActionBlock(bool IsUp) override;
	virtual void InputActionStopBlock(bool IsUp) override;
	virtual void InputActionDashLeft(bool IsUp) override;
	virtual void InputActionDashRight(bool IsUp) override;
	/** End IInputParser implementation */

	void PushInputToken(EInputToken NewToken);
	void StartComboTimer();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Gets the direciton of the opponent, return's 0 if MyFighter == nullptr */
	float GetOpponentDirection();

	/** Gets the dircetion the Fighter is facing, return's 0 if MyFighter == nullptr */
	float GetFighterFacing();
};

