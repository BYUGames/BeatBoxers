// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BeatBoxers.h"
#include "Interfaces/IFighter.h"
#include "Interfaces/IFighterWorld.h"
#include "Interfaces/IFighterState.h"
#include "Interfaces/IMoveset.h"
#include "Interfaces/IInputParser.h"
#include "Interfaces/ISoloTracker.h"
#include "FighterStateComponent.h"
#include "MovesetComponent.h"
#include "InputParserComponent.h"
#include "SoloTrackerComponent.h"
#include "FighterCharacter.generated.h"

UCLASS()
class BEATBOXERS_API AFighterCharacter : public ACharacter, public IFighter
{
	GENERATED_UCLASS_BODY()

public:
	// Do not change these to const members, it will not be enforced because of the getters.
	UFighterStateComponent *FighterState;
	UMovesetComponent *Moveset;
	UInputParserComponent *InputParser;
	USoloTrackerComponent *SoloTracker;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FFighterData FighterData;

protected:
	IFighterState *MyFighterState;
	TWeakObjectPtr<AActor> MyOpponent;
	float Facing;

	/** Windup for a jump. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float JumpDelay;

	FTimerHandle TimerHandle_Jump;
	FTimerHandle TimerHandle_Glow;

	void OnJumpTimer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	IFighter* GetSelfAsFighter();
	const IFighter* GetSelfAsFighter() const;
	IFighterState* GetFighterState();
	const IFighterState* GetFighterState() const;
	IMoveset* GetMoveset();
	const IMoveset* GetMoveset() const;
	IInputParser* GetInputParser();
	const IInputParser* GetInputParser() const;
	ISoloTracker* GetSoloTracker();
	const ISoloTracker* GetSoloTracker() const;
	IFighterWorld* GetFighterWorld();
	const IFighterWorld* GetFighterWorld() const;

	void InputAxisHorizontal(float amount);
	void InputAxisVertical(float amount);
	void InputActionUp();
	void InputActionLeft();
	void InputActionDown();
	void InputActionRight();
	void InputActionLight();
	void InputActionMedium();
	void InputActionHeavy();
	void InputActionJump();
	void SetGravityScale(float scale);

public:	
	UPROPERTY(BlueprintAssignable)
	FStartJumpEvent StartJumpEvent;

	UPROPERTY(BlueprintAssignable)
	FLandEvent LandEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects DefaultHitEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects DefaultBlockEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects JumpEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InputBufferLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ComplexInputWindow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FDataTableRowHandle DefaultMoveState;

	/** IFighter implementation */
	virtual void RegisterFighterState(TWeakObjectPtr<UObject> NewFighterState) override;
	virtual void RegisterOpponent(TWeakObjectPtr<AActor> Opponent) override;
	virtual float GetOpponentDirection() const override;
	virtual void ApplyMovement(FMovement Movement) override;
	virtual bool IsBlocking() const override;
	virtual EStance GetStance() const override;
	virtual void SetFacing(float Sign) override;
	virtual float GetFacing() const override { return Facing; }
	virtual TWeakObjectPtr<AController> GetFighterController() const override;
	virtual void SetWantsToCrouch(bool WantsToCrouch) override;
	virtual void SetMoveDirection(float Direction) override;
	virtual void Jump() override;
	virtual FStartJumpEvent& GetOnStartJumpEvent() override;
	virtual FLandEvent& GetOnLandEvent() override;
	virtual void OnInputReceived() override;
	virtual FDataTableRowHandle GetDefaultMoveState() override { return DefaultMoveState; }
	UFUNCTION(BlueprintImplementableEvent)
	void HitOnBeatLogic() override;
	UFUNCTION(BlueprintImplementableEvent)
	void InputOnBeatLogic() override;
	/** End IFighter implementation */

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Input Received"))
	void K2_OnInputReceived();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	virtual void Landed(const FHitResult& Result) override;

	virtual void PossessedBy(AController *NewController) override;

	/** Gets the sign of the actor's facing in the X axis. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Facing"))
	virtual float K2_GetFacing() const { return GetFacing(); }

	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Is Blocking"))
	virtual bool K2_IsBlocking() const;

	UFUNCTION(BlueprintPure)
	virtual float GetHorizontalMovement() const;

	void SetOpponent(TWeakObjectPtr<AActor> NewOpponent);

};
