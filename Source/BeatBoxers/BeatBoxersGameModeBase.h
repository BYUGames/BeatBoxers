// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BeatBoxersStructs.h"
#include "FighterWorld.h"
#include "BBPlayerController.h"
#include "FighterCharacter.h"
#include "BeatBoxersGameModeBase.generated.h"

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
	virtual struct FHitResult TraceHitbox(FMoveHitbox Hitbox, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TArray<class AActor*>& IgnoreActors, class AActor* Source, class AController* SourceController) override;
	/** End IFighterWorld implementation */
};
