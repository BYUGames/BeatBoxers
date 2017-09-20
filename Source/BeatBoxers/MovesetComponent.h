// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BeatBoxers.h"
#include "Moveset.h"
#include "FighterState.h"
#include "SoloTracker.h"
#include "MovesetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UMovesetComponent : public UActorComponent, public IMoveset
{
	GENERATED_UCLASS_BODY()

protected:
	IFighterState *MyFighterState;
	ISoloTracker *MySoloTracker;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	/** IMoveset implementation */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> FighterState) override;
	virtual void RegisterSoloTracker(TWeakObjectPtr<UObject> SoloTracker) override;
	virtual void ReceiveInputToken(EInputToken Token) override;
	virtual void OnWindowFinished(bool WasInterrupted) override;
	virtual void BeginSolo() override;
	virtual void OnSoloCorrect(EInputToken Token) override;
	virtual void OnSoloIncorrect() override;
	/** End IMovesetImplementation */

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
