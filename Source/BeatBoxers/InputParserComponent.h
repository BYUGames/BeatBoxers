// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "InputParser.h"
#include "Moveset.h"
#include "FighterState.h"
#include "InputParserComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UInputParserComponent : public UActorComponent, public IInputParser
{
	GENERATED_UCLASS_BODY()

protected:
	IFighterState *MyFighterState;
	IMoveset *MyMoveset;
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	/** IInputParser implementation */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) override;
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) override;
	virtual void OnControlReturned() override;
	virtual void InputAxisHorizontal(float Amount) override;
	virtual void InputAxisVertical(float Amount) override;
	virtual void InputActionDLeft(bool IsUp) override;
	virtual void InputActionDRight(bool IsUp) override;
	virtual void InputActionDDown(bool IsUp) override;
	virtual void InputActionDUp(bool IsUp) override;
	virtual void InputActionPunch(bool IsUp) override;
	virtual void InputActionKick(bool IsUp) override;
	/** End IInputParser implementation */

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
