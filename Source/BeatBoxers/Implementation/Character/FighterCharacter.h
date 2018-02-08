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
	bool bIsDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StartingHealth;

	/** Windup for a jump. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float JumpDelay;

	FTimerHandle TimerHandle_Jump;
	FTimerHandle TimerHandle_Glow;

	void OnJumpTimer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
	void InputActionSpecial1();
	void InputActionSpecial2();
	void InputActionSpecial3();
	void InputActionSuper();
	void InputActionDodge();
	void InputActionDashLeftButton();
	void InputActionDashRightButton();
	void SetGravityScale(float scale);

public:	
	UPROPERTY(BlueprintAssignable)
	FStartJumpEvent StartJumpEvent;

	UPROPERTY(BlueprintAssignable)
	FLandEvent LandEvent;

	UPROPERTY(BlueprintAssignable)
	FClashEvent ClashEvent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects DefaultHitEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects DefaultBlockEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RecoveryDuration;

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
	virtual bool IsInvulnerable() const override;
	virtual EStance GetStance() const override;
	virtual void SetFacing(float Sign) override;
	virtual float GetFacing() const override { return Facing; }
	virtual TWeakObjectPtr<AController> GetFighterController() const override;
	virtual void SetWantsToCrouch(bool WantsToCrouch) override;
	virtual void SetMoveDirection(float Direction) override;
	virtual void Jump() override;
	virtual FStartJumpEvent& GetOnStartJumpEvent() override;
	virtual FLandEvent& GetOnLandEvent() override;
	virtual FClashEvent& GetOnClashEvent() override;
	virtual void OnInputReceived() override;
	virtual FDataTableRowHandle GetDefaultMoveState() const override { return DefaultMoveState; }
	virtual void HitOnBeatLogic() override;
	virtual void InputOnBeatLogic() override { K2_InputOnBeatLogic(); }
	virtual void MissBeat() override;
	virtual bool IsJumping() override;
	virtual bool IsDead() const override;
	virtual void Knockdown() override;
	virtual FMusicBalanceParams GetMusicBalance() const override { return FighterData.MusicBalance; }
	virtual void SetFighterCollisions(bool DoesCollide) override;
	virtual void StartStun(float Duration, bool WasBlocked) override;
	virtual float GetFighterCurrentWindowAccuracy() const override;
	virtual FMoveHitbox GetFighterHitbox() const override;
	virtual bool CanClash() const override;
	virtual void Clash() override;
	virtual int GetTimesHitThisKnockdown() const override;
	virtual void AddHit() override;
	/** End IFighter implementation */

	UFUNCTION(BlueprintPure, meta=(DisplayName="Is Jumping"))
	bool K2_IsJumping() { return IsJumping(); }

	UFUNCTION(BlueprintPure, meta=(DisplayName="Is Knocked Down"))
	bool K2_IsKnockedDown();

	UFUNCTION(BlueprintPure, meta=(DisplayName="Is Dead"))
	bool K2_IsDead();

	/** HitOnBeatLogic will fire if the attack was on the beat and it hit a player*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Hit On Beat Logic"))
	void K2_HitOnBeatLogic();

	/** InputOnBeatLogic will fire when the input is within the accuracy window*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Input On Beat Logic"))
	void K2_InputOnBeatLogic();

	/** TODO: add desciption for this event */
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

	/** If the character is blocking*/
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get is Blocking"))
	virtual bool K2_IsBlocking() const;

	/** If the character is stunned by blocking (not if he is blocking in the code)*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get is Block Stunned"))
	virtual bool K2_IsBlockStunned() const;

	UFUNCTION(BlueprintPure)
	virtual float GetHorizontalMovement() const;

	void SetOpponent(TWeakObjectPtr<AActor> NewOpponent);

	UFUNCTION()
	void OnRoundEnd(int Winner);
};
