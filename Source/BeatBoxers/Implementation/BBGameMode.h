// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BeatBoxersStructs.h"
#include "Interfaces/IFighterWorld.h"
#include "BBGameMode.generated.h"

class AFighterCharacter;
class ABBPlayerController;
class ABBGameState;
class UBBGameInstance;
class IFighter;

/**
 *
 */
UCLASS()
class BEATBOXERS_API ABBGameMode : public AGameMode, public IFighterWorld
{
	GENERATED_UCLASS_BODY()
public:
	/** This is the distance scanned to determine if the target was "against the wall" when hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HitscanDistanceConstant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bDrawDebugTraces;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int RoundsToWin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelayBetweenRounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelayBeforeEnd;

	/** Defines the Effects of a Clash. Needs to always be relative. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FImpactData DefaultClashImpact;

	/** Round time in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int RoundTime;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInRound;

	bool bReadyToEnd;

	UPROPERTY(BlueprintAssignable)
	FSoloStartEvent SoloStartEvent;

	UPROPERTY(BlueprintAssignable)
	FSoloEndEvent SoloEndEvent;

	UPROPERTY(BlueprintAssignable)
	FRoundStartEvent RoundStartEvent;

	UPROPERTY(BlueprintAssignable)
	FRoundEndEvent RoundEndEvent;

	UPROPERTY(BlueprintAssignable)
	FMatchEndEvent MatchEndEvent;

	UPROPERTY(BlueprintAssignable)
	FPlayerBeatComboChangedEvent PlayerBeatComboChangedEvent;

	UPROPERTY(EditAnywhere, Meta = (BeatWindow))
	float AccuracyWindowSize = 0.2f;

	/** This actor must implement the IMusicBox interface. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AActor> DefaultMusicBoxClass;

	FTimerHandle TimerHandle_RoundEnd;
	FTimerHandle TimerHandle_StartNextRound;

	/** IFighterWorld implementation */
	virtual EFighterDamageType GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const override;
	virtual struct FHitResult TraceHitbox(FVector Source, FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) override;
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController) override;
	virtual int ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement);
	virtual void StartSolo(TWeakObjectPtr<AActor> OneSoloing) override;
	virtual void EndSolo() override;
	virtual FSoloStartEvent& GetOnSoloStartEvent() override;
	virtual FSoloEndEvent& GetOnSoloEndEvent() override;
	virtual FRoundStartEvent& GetOnRoundStartEvent() override;
	virtual FRoundEndEvent& GetOnRoundEndEvent() override;
	virtual FMatchEndEvent& GetOnMatchEndEvent() override;
	virtual float GetTimeLeftInRound() override;
	virtual FPlayerBeatComboChangedEvent& GetOnPlayerBeatComboChangedEvent() override;
	virtual void AdjustLocation(AActor* ActorToAdjust) override;
	virtual UObject* GetMusicBox() override;
	virtual bool IsOnBeat(float Accuracy) override;
	virtual bool IsInRound() override { return bIsInRound; }
	virtual float TimeToNextRound() override;
	virtual void PlayerHitOnBeat(APlayerController* PlayerController) override;
	virtual void PlayerMissBeat(APlayerController* PlayerController) override;
	virtual void OnClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB) override;
	virtual bool CheckClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB) override;
	/** End IFighterWorld implementation */

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Time Left In Round"))
	virtual float K2_GetTimeLeftInRound() { return GetTimeLeftInRound(); }

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Is In Round"))
	virtual float K2_IsInRound() { return IsInRound(); }

	virtual bool DoesBlock(IFighter *Fighter, EFighterDamageType DamageType) const;

	virtual void AddSpecial(APlayerState *PlayerState, float Amount);
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ReadyToEndMatch_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	FImpactData GetScaledImpactData(const FImpactData& ImpactData, float Accuracy);

	UFUNCTION()
	virtual void OnMusicEnd();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = AdjustLocation))
	void BPAdjustLocation(AActor *ActorToAdjust);


protected:
	virtual void SpawnPawns();
	virtual void AttachOpponents();
	virtual void AdjustCamera();

	virtual int GetWinnerIndex();

	virtual void StartRoundWithDelay();
	virtual void StartRound();
	/** Does Logic for ending the round */
	virtual void EndRound();
	virtual void EndGame(int Winner);

	virtual void PushMusicBalance();

	/** Enables or disables player input. */
	virtual void SetPlayerInput(bool IsEnabled);

	/** Callback for round timer. */
	virtual void OnRoundTimeOut();

	/** If ForWinner is true, it will return the the ImpactData for the winner, otherwise it will return the Default/Looser data. */
	virtual FImpactData GetClashImpact(bool ForWinner) { return DefaultClashImpact; }

	/** Returns nullptr if no winner, otherwise Returns the pointer to the winner. */
	virtual IFighter* DetermineClashWinner(IFighter* FighterA, IFighter* FighterB);

	/** Returns -1 on error, 1 if theactor in question was backed against a wall, 0 otherwise. */
	virtual int ApplyImpact(TWeakObjectPtr<AActor> Actor, FImpactData ImpactData, bool WasBlocked, TWeakObjectPtr<AController> SourceController, TWeakObjectPtr<AActor> Source);
};
