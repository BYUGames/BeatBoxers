// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BeatBoxersStructs.h"
#include "FighterWorld.h"
#include "BeatBoxersGameModeBase.generated.h"

class AFighterCharacter;
class ABBPlayerController;

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABeatBoxersGameModeBase : public AGameModeBase, public IFighterWorld
{
	GENERATED_UCLASS_BODY()
	
public:
	/** IFighterWorld implementation */
	virtual EFighterDamageType GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const override;
	virtual struct FHitResult TraceHitbox(FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors) override;
	virtual void HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController) override;
	/** End IFighterWorld implementation */
};
