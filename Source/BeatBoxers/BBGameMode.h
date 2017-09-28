// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BeatBoxersStructs.h"
#include "FighterWorld.h"
#include "BBGameMode.generated.h"

class AFighterCharacter;
class ABBPlayerController;
class ABBGameState;

/**
 *
 */
UCLASS()
class BEATBOXERS_API ABBGameMode : public AGameMode, public IFighterWorld
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector InitialCameraLocation;
	FVector InitialCameraLookAtLocation;

public:
	/** IFighterWorld implementation */
	virtual EFighterDamageType GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const override;
	virtual struct FHitResult TraceHitbox(FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) override;
	virtual EHitResponse HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController) override;
	virtual void ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement& Movement);
	/** End IFighterWorld implementation */

	virtual void StartMatch() override;

	ABBGameState* GetBBGameState();
};
