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

	UPROPERTY(BlueprintAssignable)
	FSoloStartEvent SoloStartEvent;

	UPROPERTY(BlueprintAssignable)
	FSoloEndEvent SoloEndEvent;

	UPROPERTY(BlueprintAssignable)
	FPlayerBeatComboChangedEvent PlayerBeatComboChangedEvent;

	UPROPERTY(EditAnywhere, Meta = (BeatWindow))
	float AccuracyWindowSize = 0.2f;

	/** This actor must implement the IMusicBox interface. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AActor> DefaultMusicBoxClass;

	/** IFighterWorld implementation */
	virtual EFighterDamageType GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const override;
	virtual struct FHitResult TraceHitbox(FVector Source, FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) override;
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController) override;
	virtual int ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement);
	virtual void StartSolo(TWeakObjectPtr<AActor> OneSoloing) override;
	virtual void EndSolo() override;
	virtual FSoloStartEvent& GetOnSoloStartEvent() override;
	virtual FSoloEndEvent& GetOnSoloEndEvent() override;
	virtual FPlayerBeatComboChangedEvent& GetOnPlayerBeatComboChangedEvent() override;
	virtual void AdjustLocation(AActor* ActorToAdjust) override;
	virtual UObject* GetMusicBox() override;
	virtual bool IsOnBeat(float Accuracy) override;
	virtual void PlayerHitOnBeat(APlayerController* PlayerController) override;
	virtual void PlayerMissBeat(APlayerController* PlayerController) override;
	/** End IFighterWorld implementation */

	virtual bool DoesBlock(IFighter *Fighter, EFighterDamageType DamageType) const;

	virtual void AddSpecial(APlayerState *PlayerState, float Amount);
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void AdjustCamera();

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

	/** Does Logic for ending the round */
	virtual void EndRound();
};
