// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.generated.h"

UENUM(BlueprintType)
enum class EFilter : uint8
{
	FE_Allowed		UMETA(DisplayName = "Allowed"),
	FE_Required		UMETA(DisplayName = "Required"),
	FE_Prohibited	UMETA(DisplayName = "Prohibited")
};

USTRUCT(BlueprintType)
struct FStanceFilter
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFilter Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFilter Crouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFilter Jumping;
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	SE_NA			UMETA(DisplayName = "N/A"),
	SE_Standing		UMETA(DisplayName = "Standing"),
	SE_Crouching	UMETA(DisplayName = "Crouching"),
	SE_Jumping		UMETA(DisplayName = "Jumping")
};

UENUM(BlueprintType)
enum class EFighterDamageType : uint8
{
	DE_None		UMETA(DisplayName = "None"),
	DE_High		UMETA(DisplayName = "High"),
	DE_Low		UMETA(DisplayName = "Low"),
	DE_Overhead	UMETA(DisplayName = "Overhead")
};

UENUM(BlueprintType)
enum class EInputToken : uint8
{
	IE_None			UMETA(DisplayName = "None"),
	IE_Punch		UMETA(DisplayName = "Punch"),
	IE_Kick			UMETA(DisplayName = "Kick"),
	IE_Jump			UMETA(DisplayName = "Jump"),
	IE_DashLeft		UMETA(DisplayName = "DashLeft"),
	IE_DashRight	UMETA(DisplayName = "DashRight")
};

USTRUCT(BlueprintType)
struct FEffects
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* SoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;
};

USTRUCT(BlueprintType)
struct FMovement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Delta;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 IsRelativeToAttackerFacing : 1;
};

USTRUCT(BlueprintType)
struct FMoveHitbox
{
	GENERATED_USTRUCT_BODY()

	/** Where to begin the trace from, relative to attacker's position and facing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Origin;

	/** Where to end the trace, relative to attacker's position and facing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector End;

	/** Radius to trace. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
};

USTRUCT(BlueprintType)
struct FImpactData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovement ImpartedMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffects SFX;
};

USTRUCT(BlueprintType)
struct FMoveWindow
{
	GENERATED_USTRUCT_BODY()

	/** How long this window lasts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	/** Whether this window, and move, can be interrupted by taking damage during it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 Interruptible : 1;

	/** Movement applied to the attacker when entering this window. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMovement AttackerMovement;

	/** Whether this window has an active hitbox. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 IsHitboxActive : 1;

	/** Hitbox used during this window. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMoveHitbox Hitbox;

	/** Impact data used when a defender is hit during this window. Transform relative to the impact point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FImpactData DefenderHit;

	/** Impact data used when a defender it hit, but successfully blocked, during this window. Transform relative to the impact point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FImpactData DefenderBlock;

	/** Effects played when entering this window. Transform relative to attacker. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffects SFX;

	/** Whether landing on the ground during this window immediately ends it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 LandingEndsWindow : 1;

	/** Whether landing on the ground during this window immediately interrupts the move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 LandingInterrupts : 1;
};

USTRUCT(BlueprintType)
struct FMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMoveWindow> Windows;

	/** Stance required to perform this move. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FStanceFilter Stance;

	/** How much special is required and consumed in perfoming this move. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpecialCost;

	/** If this move does damage other than the type corresponding to the stance the attacker is in, specify that damage type here. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFighterDamageType DamageTypeOverride;
};

USTRUCT(BlueprintType)
struct FSoloParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfNotes;
};
