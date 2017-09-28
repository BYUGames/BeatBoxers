// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.generated.h"

template<typename TEnum>
static FORCEINLINE FString GetEnumValueToString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	return enumPtr->GetNameByValue((int64)Value).ToString();
}

UENUM(BlueprintType)
enum class EFilter : uint8
{
	FE_Allowed		UMETA(DisplayName = "Allowed"),
	FE_Required		UMETA(DisplayName = "Required"),
	FE_Prohibited	UMETA(DisplayName = "Prohibited")
};

UENUM(BlueprintType)
enum class EHitResponse : uint8
{
	HE_Hit		UMETA(DisplayName = "Hit"),
	HE_Blocked	UMETA(DisplayName = "Blocked"),
	HE_Missed	UMETA(DisplayName = "Missed")
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
	uint32 AttachToActor : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;
};

USTRUCT(BlueprintType)
struct FMovement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector Delta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 IsRelativeToAttackerFacing : 1;

	bool IsValid() const;

	operator bool() const{ return IsValid(); }
};

USTRUCT(BlueprintType)
struct FMoveHitbox
{
	GENERATED_USTRUCT_BODY()

	/** Where to begin the trace from, relative to attacker's position and facing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector Origin;

	/** Where to end the trace, relative to attacker's position and facing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector End;

	/** Radius to trace. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius;
};

USTRUCT(BlueprintType)
struct FImpactData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMovement ImpartedMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StunLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects SFX;
};

USTRUCT(BlueprintType)
struct FMoveWindow
{
	GENERATED_USTRUCT_BODY()

	/** For convenience, how long this window lasts before enabling its hitbox and starting its duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Windup;

	/** How long this window lasts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration;

	/** For convenience, how long this window lasts after its duration has expired and it has disabled its hitbox. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Winddown;

	/** Whether this window, and move, can be interrupted by taking damage during it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 Interruptible : 1;

	/** Whether or not landing a blow during this window starts the character's solo. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 BeginsSolo : 1;

	/** If this window does damage other than the type corresponding to the stance the attacker is in, specify that damage type here. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFighterDamageType DamageTypeOverride;

	/** Movement applied to the attacker when entering this window. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMovement AttackerMovement;

	/** Whether this window has an active hitbox. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 IsHitboxActive : 1;

	/** Hitbox used during this window. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMoveHitbox Hitbox;

	/** Impact data used when a defender is hit during this window. Transform relative to the impact point, can't be attached. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FImpactData DefenderHit;

	/** Impact data used when a defender it hit, but successfully blocked, during this window. Transform relative to the impact point, can't be attached. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FImpactData DefenderBlock;

	/** Effects played when this window misses. Transform relative to attacker, attaches to attacker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects MissSFX;

	/** Effects played when entering this window. Transform relative to attacker, attaches to attacker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects SFX;

	/** Whether landing on the ground during this window immediately ends it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 LandingEndsWindow : 1;

	/** Whether landing on the ground during this window immediately interrupts the move. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 LandingInterrupts : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Animation;
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
};

USTRUCT(BlueprintType)
struct FSoloParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfNotes;
};
