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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector InitialCameraLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector InitialCameraLookAtLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bDrawDebugTraces;

	UPROPERTY(BlueprintAssignable)
	FSoloStartEvent SoloStartEvent;

	UPROPERTY(BlueprintAssignable)
	FSoloEndEvent SoloEndEvent;

	UPROPERTY(EditAnywhere, Meta = (BeatWindow))
	float AccuracyRestraint = 0.9f;

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
	virtual void AdjustLocation(AActor* ActorToAdjust) override;
	virtual UObject* GetMusicBox() override;
	virtual bool IsOnBeat(float Accuracy) override;
	/** End IFighterWorld implementation */

	virtual bool DoesBlock(IFighter *Fighter, EFighterDamageType DamageType) const;

	virtual void AddSpecial(APlayerState *PlayerState, float Amount);
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;

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
};
