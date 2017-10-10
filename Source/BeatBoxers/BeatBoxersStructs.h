// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

UENUM(BlueprintType)
enum class EStance : uint8
{
	SE_NA			UMETA(DisplayName = "N/A"),
	SE_Standing		UMETA(DisplayName = "Standing"),
	SE_Crouching	UMETA(DisplayName = "Crouching"),
	SE_Jumping		UMETA(DisplayName = "Jumping")
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

	bool FilterStance(EStance Stance) const;
};

UENUM(BlueprintType)
enum class EWindowEnd : uint8
{
	WE_Finished		UMETA(DisplayName = "Finished Normally"),
	WE_Stunned		UMETA(DisplayName = "Interrupted by Stun"),
	WE_LandInt		UMETA(DisplayName = "Interrupted by Landing"),
	WE_LandSkip		UMETA(DisplayName = "Ended Early by Landing")
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
struct FInputTokenBools
{
	GENERATED_USTRUCT_BODY()

	/** Overrides all other options. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Punch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Kick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Jump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DashLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DashRight;

	bool FilterInputToken(EInputToken Token) const;
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

	FString ToString() const;
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
};

USTRUCT(BlueprintType)
struct FSoloParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfNotes;
};

UCLASS(Abstract, MinimalAPI, Blueprintable)
class AMoveState : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	/** Which inputs will trigger this move, assuming stance and special conditions are met. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FInputTokenBools AllowedInputs;

	/** This is what allows or prevents this move in certain stances, like standing or crouching. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FStanceFilter StanceFilter;

	/** Amount of special required, and consumed, to use the move. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpecialCost;

	/** Amount of time after finishing last window that the MovesetComponent will remain in this state until reverting to its default state. Negative means it stays in the state indefinitely. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxDuration;

	/** This defines the actual things done in this move. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FMoveWindow> MoveWindows;

	/** Possible moves to go to from here. Will proceed down the list in order and select the first available move. If none are available resets MovesetComponent to default state. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray< TSubclassOf<AMoveState> > PossibleTransitions;
};

UENUM(BlueprintType)
enum class ENoteType : uint8
{
	NE_None		UMETA(DisplayName = "None")
};

/** Results returned to the moveset from the fretboard. */
USTRUCT(BlueprintType)
struct FFretboardInputResult
{
	GENERATED_USTRUCT_BODY()

	/** Was the input correct. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 WasCorrect : 1;

	/** Positive means they were late, negative means early. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeDifference;
};

/** Representation of notes sent to the fretboard. */
USTRUCT(BlueprintType)
struct FNoteData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENoteType NoteType;

	/** Time until this note passes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLeft;
};

USTRUCT(BlueprintType)
struct FNewGameData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Player0Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Player1Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 IsSecondPlayerHuman : 1;
};