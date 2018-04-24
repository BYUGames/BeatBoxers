// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "PaperFlipbookComponent.h"
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
enum class ERPSType : uint8
{
	RPS_None		UMETA(DisplayName = "None")
	, RPS_Attack	UMETA(DisplayName = "Attack")
	, RPS_Grab		UMETA(DisplayName = "Grab")
	, RPS_Block		UMETA(DisplayName = "Block")
};

// Returns winner: 0 for tie, 1 for a, 2 for b
int GetRPSWinner(ERPSType a, ERPSType b);

UENUM(BlueprintType)
enum class EWindowStage : uint8
{
	WE_Windup			UMETA(DisplayName = "Wind up")
	, WE_Duration		UMETA(DisplayName = "Duration")
	, WE_Winddown		UMETA(DisplayName = "Wind down")
	, WE_None			UMETA(DisplayName = "None")
};

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
	HE_Hit			UMETA(DisplayName = "Hit"),
	HE_Blocked		UMETA(DisplayName = "Blocked"),
	HE_Missed		UMETA(DisplayName = "Missed"),
	HE_Clashed		UMETA(DisplayName = "Clash")
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
	WE_Hit		UMETA(DisplayName = "Finished and hit"),
	WE_Missed		UMETA(DisplayName = "Finished and missed/blocked/no hitbox"),
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
	IE_Light		UMETA(DisplayName = "Light"),
	IE_LightForward	UMETA(DisplayName = "LightForward"),
	IE_LightBack	UMETA(DisplayName = "LightBack"),
	IE_LightCrouch	UMETA(DisplayName = "LightCrouch"),
	IE_Medium		UMETA(DisplayName = "Medium"),
	IE_MediumCrouch	UMETA(DisplayName = "MediumCrouch"),
	IE_HeavyCrouch	UMETA(DisplayName = "HeavyCrouch"),
	IE_Heavy		UMETA(DisplayName = "Heavy"),
	IE_Special1     UMETA(DisplayName = "Special1"),
	IE_Special2     UMETA(DisplayName = "Special2"),
	IE_Special3     UMETA(DisplayName = "Special3"),
	IE_Super		UMETA(DisplayName = "Super"),
	IE_Jump			UMETA(DisplayName = "Jump"),
	IE_DashForward	UMETA(DisplayName = "DashForward"),
	IE_DashBackward	UMETA(DisplayName = "DashBackward"),
	IE_Block        UMETA(DisplayName = "Block"),
	IE_Up			UMETA(DisplayName = "Up"),
	IE_Down			UMETA(DisplayName = "Down"),
	IE_Left			UMETA(DisplayName = "Left"),
	IE_Right		UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class ENoteType : uint8
{
	NE_None			UMETA(DisplayName = "None"),
	NE_Light		UMETA(DisplayName = "Light"),
	NE_LightForward	UMETA(DisplayName = "LightForward"),
	NE_LightBack	UMETA(DisplayName = "LightBack"),
	NE_LightCrouch	UMETA(DisplayName = "LightCrouch"),
	NE_Medium		UMETA(DisplayName = "Medium"),
	NE_MediumCrouch	UMETA(DisplayName = "MediumCrouch"),
	NE_Heavy		UMETA(DisplayName = "Heavy"),
	NE_HeavyCrouch	UMETA(DisplayName = "HeavyCrouch"),
	NE_Up			UMETA(DisplayName = "Up"),
	NE_Down			UMETA(DisplayName = "Down"),
	NE_Left			UMETA(DisplayName = "Left"),
	NE_Right		UMETA(DisplayName = "Right")
};


USTRUCT(BlueprintType)
struct FInputTokenBools
{
	GENERATED_USTRUCT_BODY()

	/** Overrides all other options. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Light;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LightForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LightBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LightCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Medium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool MediumCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Heavy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HeavyCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Special1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Special2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Special3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Super;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Jump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Block;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DashForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DashBackward;

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

	bool IsValid() const;

	FEffects() :
		ParticleSystem(nullptr)
		, SoundCue(nullptr)
		, AttachToActor(false)
		, RelativeTransform(FTransform::Identity)
	{}
};

USTRUCT(BlueprintType)
struct FMovement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D Delta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 IsRelativeToAttackerFacing : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 UseDeltaAsSpeed : 1;

	/** Ignores Duration, assumes UseDeltaAsSpeed, applies a physics-based launch. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 UsePhysicsLaunch : 1;

	/** Vector used to launch opponent using this move if they're in knockdown */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D InAirLaunchDelta;

	bool IsValid() const;

	operator bool() const{ return IsValid(); }

	FMovement operator-();

	FMovement operator*(float f);

	FString ToString() const;

	FMovement() :
		Delta(FVector2D(0.f, 0.f))
		, Duration(0.f)
		, IsRelativeToAttackerFacing(true)
		, UseDeltaAsSpeed(false)
		, UsePhysicsLaunch(false)
		, InAirLaunchDelta(FVector2D(0.f, 0.f))
	{}
};

USTRUCT(BlueprintType)
struct FMusicBalanceParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMaestraOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRiffOn;

	FMusicBalanceParams() : bIsMaestraOn(false), bIsRiffOn(false) {}

	FMusicBalanceParams operator+(const FMusicBalanceParams& rhs);
	FMusicBalanceParams& operator+=(const FMusicBalanceParams& rhs);
};

USTRUCT(BlueprintType)
struct FMoveHitbox
{
	GENERATED_USTRUCT_BODY()

	/** Where to begin the trace from, relative to attacker's position and facing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D Origin;

	/** Where to end the trace, relative to attacker's position and facing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D End;

	/** Radius to trace. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius;

	/** Value Representing Rock Paper or Scissors, to determine which beats the other */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ERPSType RPSCategory;

	FMoveHitbox()
	{
		RPSCategory = ERPSType::RPS_None;
		Radius = 20.f;
		End.X = 75.f;
	}
};

USTRUCT(BlueprintType)
struct FImpactData
{
	GENERATED_USTRUCT_BODY()

	/** Whether this impact ends the current move window, skipping whatever duration or winddown remains. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 EndsCurrentWindow : 1;

	/** Whether this impact sets the target into a konckdown state. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 bKnocksDown : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMovement ImpartedMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SpecialGenerated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StunLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects SFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HitstopAmount;

	FImpactData() :
		EndsCurrentWindow(false)
		, bKnocksDown(false)
		, ImpartedMovement(FMovement())
		, Damage(0.f)
		, SpecialGenerated(0.f)
		, StunLength(0.f)
		, SFX(FEffects())
		, HitstopAmount(0.f)
	{}
};

USTRUCT(BlueprintType)
struct FMoveWindow
{
	GENERATED_USTRUCT_BODY()

	/** Whether this window requires the last window to have hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 RequiresLastWindowHit : 1;

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

	/** Whether this window reduces the hitbox of the attacker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 CrouchAttack : 1;

	/** Whether or not landing a blow during this window starts the character's solo. If the blow is blocked the solo does not start. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 BeginsSolo : 1;

	/** If this window does damage other than the type corresponding to the stance the attacker is in, specify that damage type here. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFighterDamageType DamageTypeOverride;

	/** Movement applied to the attacker as it enters windup. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMovement AttackerMovement;

	/** Whether this window has an active hitbox during it's duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 IsHitboxActive : 1;

	/** Hitbox used during this window's duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FMoveHitbox Hitbox;

	/** Will gravity be scaled during this windows duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 IsGravityScaled : 1;

	/** Multiplier of gravity for the attacker during the windows duration. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GravityScale;

	/** Impact data used when a defender is hit during this window. Transform relative to the impact point, can't be attached. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FImpactData DefenderHit;

	/** Impact data used when a defender it hit, but successfully blocked, during this window. Transform relative to the impact point, can't be attached. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FImpactData DefenderBlock;

	/** Effects played when this duration ends without connecting. Transform relative to attacker, attaches to attacker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects MissSFX;

	/** Effects played when entering this window's duration. Transform relative to attacker, attaches to attacker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects WindupSFX;

	/** Effects played when entering this window's duration. Transform relative to attacker, attaches to attacker. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FEffects SFX;

	/** Whether landing on the ground during this window immediately ends it (combos off it can continue), still plays winddown. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 LandingEndsWindow : 1;

	/** Whether landing on the ground during this window immediately interrupts the move (combo), still plays winddown. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 LandingInterrupts : 1;

	/** The name of the row in the fighter's move animation table to find the animation montage to play in conjuction with this window, starts when entering windup. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName AnimName;

	/** If true, the player will ignore collisions during the window. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 IgnoreCollisions : 1;

	/** If true, the player will be invincible during the window. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 Invincibility : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint32 Grab : 1;

	FMoveWindow()
	{
		Interruptible = true;
	}
};

USTRUCT(BlueprintType)
struct FMoveAnimation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage *AntisipationAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage *ExecutionAnimMontage;
};

USTRUCT(BlueprintType)
struct FMoveFlipbookAnimation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook *AnimFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FlipbookOffsetHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FlipbookScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FlipbookPlayRate;


};

USTRUCT(BlueprintType)
struct FSoloParameters
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfNotes;
};

USTRUCT(BlueprintType)
struct FMoveData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	/** Which inputs will trigger this move, assuming stance and special conditions are met. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInputTokenBools AllowedInputs;

	/** Whether the move requires being on beat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint16 bRequiresOnBeat: 1;

	/** This is what allows or prevents this move in certain stances, like standing or crouching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStanceFilter StanceFilter;

	/** Amount of special required, and consumed, to use the move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpecialCost;

	/** Amount of time after finishing last window that the MovesetComponent will remain in this state until reverting to its default state. Negative means it stays in the state indefinitely. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxPostWait;

	/** This defines the actual things done in this move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMoveWindow> MoveWindows;

	/** Possible other moves to go to from here. Will proceed down the list in order and select the first available move. If none are available resets MovesetComponent to default state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDataTableRowHandle> PossibleTransitions;
};

/** Results returned to the moveset from the fretboard. */
USTRUCT(BlueprintType)
struct FFretboardInputResult
{
	GENERATED_USTRUCT_BODY()

	/** 1 is perfect accuracy, 0 is worst accuracy, -1 was a miss. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Accuracy;
};

USTRUCT(BlueprintType)
struct FNoteData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENoteType NoteType;

	/** Which track is the note on? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Track;

	/** Total time this note had to start with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialDuration;

	/** Timer for this note. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle TimerHandle;
};

USTRUCT(BlueprintType)
struct FFeedNoteData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENoteType NoteType;

	/** Which track is the note on? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Track;

	/** Total time this note had to start with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialDuration;

	/** Total time this note had to start with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationRemaining;
};

USTRUCT(BlueprintType)
struct FNoteWidgetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENoteType NoteType;

	/** Which track is the note on? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Track;

	/** Percent of distance travelled until the note "hits". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Percent;
};

USTRUCT(BlueprintType)
struct FNewGameData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SongName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Player0Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Player1Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 Player0Tint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 Player1Tint;

};

USTRUCT(BlueprintType)
struct FFighterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FighterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture *Portrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture *InGamePortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture *StylizedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMusicBalanceParams MusicBalance;
};

USTRUCT(BlueprintType)
struct FBufferInputToken
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInputToken token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float accuracy;

	FBufferInputToken operator=(const FBufferInputToken toSet)
	{
		token = toSet.token;
		accuracy = toSet.accuracy;
		return toSet;
	}

	FBufferInputToken()
	{
		token = EInputToken::IE_None;
		accuracy = -1;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewFeedNoteEvent, FFeedNoteData, NoteData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewNoteEvent, int, NoteID, FNoteData, NoteData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNoteEndEvent, int, NoteID, float, Accuracy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotesClearedEvent);