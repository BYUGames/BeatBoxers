// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "BeatBoxers.h"
#include "Interfaces/IFighterState.h"
#include "Interfaces/IFighter.h"
#include "Interfaces/IFighterWorld.h"
#include "Interfaces/IMoveset.h"
#include "Interfaces/IInputParser.h"
#include "Interfaces/IFighterPlayerState.h"
#include "BBFunctionLibrary.h"
#include "FighterStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATBOXERS_API UFighterStateComponent : public UActorComponent, public IFighterState
{
	GENERATED_UCLASS_BODY()

protected:
	IFighterWorld *MyFighterWorld;
	IFighter *MyFighter;
	IMoveset *MyMoveset;
	IInputParser *MyInputParser;
	IFighterPlayerState *MyFighterPlayerState;

	uint32 bIsHitboxActive : 1;
	uint32 bHasMoveWindowHit : 1;
	uint32 bIsBeingMoved : 1;
	uint32 bIsCurrentStunBlock : 1;
	uint32 bIsBlockButtonDown : 1;
	uint32 bIsFrozenForSolo : 1;
	uint32 bWantsToCharge: 1;
	uint32 bIsKnockedDown : 1;

	EWindowStage CurrentWindowStage;
	EWindowEnd CurrentWindowEnd;
	FMovement CurrentMovement;
	FMoveWindow CurrentWindow;
	TArray<TWeakObjectPtr<AActor>> ActorsToIgnore;
	float MoveDirection;
	float VerticalDirection;
	float HorizontalDirection;
	float CurrentWindowAccuracy;
	int TimesHitThisKnockdown;

	UPROPERTY(EditDefaultsOnly)
	FEffects DefaultHitEffects;

	UPROPERTY(EditDefaultsOnly)
	FEffects DefaultBlockEffects;

	FTimerHandle TimerHandle_Window;
	FTimerHandle TimerHandle_Stun;
	FTimerHandle TimerHandle_Movement;
	FTimerHandle TimerHandle_Invulnerable;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Sets the window timer to the window's windup time to call OnWindowWindupFinished.
	void StartCurrentWindowWindup();

	// Calls StartCurrentWindow.
	void OnCurrentWindowWindupFinished();

	// Sets the window timer to the window's duration to call OnWindowFinished, sets up hitbox.
	void StartCurrentWindowDuration();

	// Calls StartCurrentWindowWinddown, disables hitbox.
	void OnCurrentWindowDurationFinished();

	// Sets the window timer to the window's winddown duration to call OnWindowWinddownFinished.
	void StartCurrentWindowWinddown();

	// Tells Moveset the window has finished.
	void OnCurrentWindowWinddownFinished();

	// Checks to see if we're mid-window, cleans up if we are and informs Moveset.
	void EndWindow(EWindowEnd WindowEnd);

	// Requests the FighterWorld perform a scan.
	void PerformHitboxScan();

	// Tells input parser control has been returned.
	void OnStunFinished();

	// Plays effects relative to the attacker.
	void PlayerAttackerEffects(FEffects& Effects);

	// Attempts to disable tick.
	void TryDisableTick();

	// Called when the movement timer expires.
	void OnMovementTimer();

	// Sets player invulnerable and sets the timer.
	void StartInvulnerableTimer(float Duration);

	// Called when knockdown timer expires.
	void OnInvulnerableTimer();

	bool MovementStep(float DeltaTime);

	void AdjustGravity(float Scale);

public:	
	/** IFighterState implementation */
	virtual void RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld) override;
	virtual void RegisterFighter(TWeakObjectPtr<UObject> Fighter) override;
	virtual void RegisterMoveset(TWeakObjectPtr<UObject> Moveset) override;
	virtual void RegisterInputParser(TWeakObjectPtr<UObject> InputParser) override;
	virtual void RegisterFighterPlayerState(TWeakObjectPtr<UObject> FighterPlayerState) override;
	virtual bool IsInputBlocked() const override;
	virtual bool IsBlocking() const override;
	virtual bool IsCharging() const override;
	virtual bool IsStunned() const override;
	virtual bool IsBlockStunned() const override;
	virtual bool IsKnockedDown() const override;
	virtual bool IsInvulnerable() const override;
	virtual bool IsMidMove() const override;
	virtual void StartMoveWindow(FMoveWindow& Window, float Accuracy) override;
	virtual void StartStun(float Duration, bool WasBlocked) override;
	virtual void SetMoveDirection(float Direction) override;
	virtual void SetVerticalDirection(float Direction) override;
	virtual void SetHorizontalDirection(float Direction) override;
	virtual void SetWantsToCrouch(bool WantsToCrouch) override;
	virtual void SetWantsToCharge(bool WantsToCharge) override;
	virtual void ApplyMovement(FMovement Movement) override;
	virtual void Jump() override;
	virtual void Block() override;
	virtual void StopBlock() override;
	virtual void OnLand() override;
	virtual float GetSpecial() const override;
	virtual void AddSpecial(float Amount) override;
	virtual bool UseSpecial(float Amount) override;
	virtual EStance GetStance() const override;
	virtual float GetCurrentHorizontalMovement() const override;
	virtual float GetCurrentVerticalDirection() const override;
	virtual float GetCurrentHorizontalDirection() const override;
	virtual void EndSolo() override;
	virtual void Knockdown() override;
	virtual void KnockdownRecovery(float Duration) override;
	virtual bool IsIgnoringCollision() const override;
	virtual float GetCurrentWindowAccuracy() const override;
	virtual bool DoesWindowUseHitbox() const override;
	virtual FMoveHitbox GetHitbox() const override;
	virtual EWindowStage GetWindowStage() const override;
	virtual int GetTimesHitThisKnockdown() const override;
	virtual void AddHit() override;
	/** End IFighterState implmementation */

	AController* GetOwnerController() const;

	UFUNCTION()
	void OnSoloStart(AActor *ActorSoloing);

	UFUNCTION()
	void OnSoloEnd();

	UFUNCTION()
	void OnBeatWindowClose();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
