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
	TWeakObjectPtr<AController> AfterHitstopSourceController;
	TWeakObjectPtr<AActor> AfterHitstopActor;
	TWeakObjectPtr<AActor> AfterHitstopSource;

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "Hitstop Events"))
	void HitstopEvents(EFighterDamageType DamageType, FImpactData Hit, FImpactData Block, float Accuracy, float HitstopAmount, int OpponentIndex, ERPSType RPSType, bool WasBlocked);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Events After Hitstop"))
	void EventsAfterHitstop(EFighterDamageType DamageType, FImpactData Hit, FImpactData Block, float Accuracy, ERPSType RPSType);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelayBeforeCombat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FightStartTime;

	/** For each subsequent hit after being knocked down, the impact will be scaled by this repeatedly. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ComboImpactScaling;

	/** Defines the Effects of a Clash. Needs to always be relative. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FImpactData DefaultClashImpact;

	/** Round time in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int RoundTime;

	/** Game does not end. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bDebugMode;

	/** Scales all impacts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GlobalImpactScaling;

	/** Scales all damage for easier testing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GlobalDamageScaling;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInRound;

	UPROPERTY(BlueprintReadOnly)
	int RoundNumber;

	bool bReadyToEnd;

	UPROPERTY(BlueprintAssignable)
	FSoloStartEvent SoloStartEvent;

	UPROPERTY(BlueprintAssignable)
	FSoloEndEvent SoloEndEvent;

	UPROPERTY(BlueprintAssignable)
	FRoundStartEvent RoundStartEvent;

	UPROPERTY(BlueprintAssignable)
	FCombatStartEvent CombatStartEvent;

	UPROPERTY(BlueprintAssignable)
	FFightStartedEvent FightStartedEvent;

	UPROPERTY(BlueprintAssignable)
	FRoundEndEvent RoundEndEvent;

	UPROPERTY(BlueprintAssignable)
	FMatchEndEvent MatchEndEvent;

	UPROPERTY(BlueprintAssignable)
	FBeatWindowCloseEvent BeatWindowCloseEvent;

	UPROPERTY(BlueprintAssignable)
	FPlayerBeatComboChangedEvent PlayerBeatComboChangedEvent;

	/** Maximum time, in seconds, a move can be early and still considered on beat. */
	UPROPERTY(EditAnywhere, Meta = (BeatWindow))
	float BeforeBeatAccuracyWindow = 0.1f;

	/** Maximum time, in seconds, a move can be late and still considered on beat. */
	UPROPERTY(EditAnywhere, Meta = (BeatWindow))
	float AfterBeatAccuracyWindow = 0.2f;

	/** This actor must implement the IMusicBox interface. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AActor> DefaultMusicBoxClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCameraShake> CameraShake;

	FTimerHandle TimerHandle_RoundEnd;
	FTimerHandle TimerHandle_StartNextRound;
	FTimerHandle TimerHandle_BeatWindowClose;
	FTimerHandle TimerHandle_StartCombat;
	FTimerHandle TimerHandle_FightStarted;
	FTimerHandle TimerHandle_Hitstop;

	/** IFighterWorld implementation */
	virtual EFighterDamageType GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const override;
	virtual struct FHitResult TraceHitbox(FVector Source, FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) override;
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, bool grab, ERPSType RPSType) override;
	virtual int ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement);
	virtual void StartSolo(TWeakObjectPtr<AActor> OneSoloing) override;
	virtual void EndSolo() override;
	virtual FSoloStartEvent& GetOnSoloStartEvent() override;
	virtual FSoloEndEvent& GetOnSoloEndEvent() override;
	virtual FRoundStartEvent& GetOnRoundStartEvent() override;
	virtual FCombatStartEvent& GetOnCombatStartEvent() override;
	virtual FFightStartedEvent& GetOnFightStartedEvent() override;
	virtual FRoundEndEvent& GetOnRoundEndEvent() override;
	virtual FMatchEndEvent& GetOnMatchEndEvent() override;
	virtual FBeatWindowCloseEvent&  GetOnBeatWindowCloseEvent() override;
	virtual float GetTimeLeftInRound() override;
	virtual FPlayerBeatComboChangedEvent& GetOnPlayerBeatComboChangedEvent() override;
	virtual void AdjustLocation(AActor* ActorToAdjust) override;
	virtual UObject* GetMusicBox() override;
	virtual bool IsOnBeat(float Accuracy) override;
	virtual bool IsInRound() override { return bIsInRound; }
	virtual float TimeToNextRound() override;
	virtual void PlayerHitOnBeat(APlayerController* PlayerController) override;
	virtual void PlayerMissBeat(APlayerController* PlayerController) override;
	virtual int OnClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB) override;
	virtual bool CheckClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB) override;
	virtual float GetScaledTime(float time)  override;
	/** End IFighterWorld implementation */

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Time Left In Round"))
	virtual float K2_GetTimeLeftInRound() { return GetTimeLeftInRound(); }

	UFUNCTION(BlueprintCallable, meta=(DisplayName="Is In Round"))
	virtual float K2_IsInRound() { return IsInRound(); }

	virtual bool DoesBlock(IFighter *Fighter, EFighterDamageType DamageType, ERPSType RPS) const;

	virtual void AddSpecial(APlayerState *PlayerState, float Amount);
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ReadyToEndMatch_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	FImpactData GetScaledImpactData(AActor *Target, const FImpactData& ImpactData, float Accuracy);

	UFUNCTION(Exec)
	void SetDebug(bool bIsDebug) { bDebugMode = bIsDebug; }

	UFUNCTION(Exec)
	void SetImpactScale(float Scale) { GlobalImpactScaling = Scale; }

	UFUNCTION()
	virtual void OnMusicEnd();

	UFUNCTION()
	virtual void OnBeat();
	virtual void BeatWindowClose();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = AdjustLocation))
	void BPAdjustLocation(AActor *ActorToAdjust);

	FMusicBalanceParams GetMusicBalance();


	

protected:
	virtual void SpawnPawns();
	virtual void AttachOpponents();
	virtual void AdjustCamera();

	virtual int GetWinnerIndex();

	virtual void StartRoundWithDelay();
	virtual void StartRound();
	/** Enable input and start timer for round after round text and delay */
	virtual void StartCombat();
	/** Let blueprint know after the fight started (to hide fight text) */
	virtual void StartedFight();
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
