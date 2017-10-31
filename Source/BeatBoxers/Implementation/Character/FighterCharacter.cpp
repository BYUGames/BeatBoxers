// copyright 2017 BYU Animation

#include "FighterCharacter.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "GameFramework/PlayerState.h"
#include "Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


// Sets default values
AFighterCharacter::AFighterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FighterState = CreateDefaultSubobject<UFighterStateComponent>(TEXT("FighterState"));
	Moveset = CreateDefaultSubobject<UMovesetComponent>(TEXT("Moveset"));
	InputParser = CreateDefaultSubobject<UInputParserComponent>(TEXT("InputParser"));
	SoloTracker = CreateDefaultSubobject<USoloTrackerComponent>(TEXT("SoloTracker"));

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate.Yaw = -1.f;
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->JumpZVelocity = 840.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 0.f;
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;

	GetCapsuleComponent()->SetHiddenInGame(false);

	JumpDelay = 0.2f;

	Facing = 1.f;

	InputBufferLength = 0.5f;
	ComplexInputWindow = 0.5f;
}

// Called when the game starts or when spawned
void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyOpponent != nullptr)
	{
		float NewFacing = FMath::Sign(MyOpponent->GetActorLocation().X - GetActorLocation().X);
		if (NewFacing != 0 && NewFacing != Facing)
		{
			Facing = NewFacing;
			SetActorRotation(FRotator(0, (Facing > 0) ? 0 : 180.f, 0));
		}
	}
}

// Called to bind functionality to input
void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Horizontal", this, &AFighterCharacter::InputAxisHorizontal);
	InputComponent->BindAxis("Vertical", this, &AFighterCharacter::InputAxisVertical);
	InputComponent->BindAction("Up", IE_Pressed, this, &AFighterCharacter::InputActionUp);
	InputComponent->BindAction("Left", IE_Pressed, this, &AFighterCharacter::InputActionLeft);
	InputComponent->BindAction("Down", IE_Pressed, this, &AFighterCharacter::InputActionDown);
	InputComponent->BindAction("Right", IE_Pressed, this, &AFighterCharacter::InputActionRight);
	InputComponent->BindAction("Light", IE_Pressed, this, &AFighterCharacter::InputActionLight);
	InputComponent->BindAction("Medium", IE_Pressed, this, &AFighterCharacter::InputActionMedium);
	InputComponent->BindAction("Heavy", IE_Pressed, this, &AFighterCharacter::InputActionHeavy);
}


IFighter* AFighterCharacter::GetSelfAsFighter()
{
	return const_cast<IFighter*>(static_cast<const AFighterCharacter*>(this)->GetSelfAsFighter());
}
const IFighter* AFighterCharacter::GetSelfAsFighter() const
{
	return Cast<IFighter>(this);
}

IFighterWorld* AFighterCharacter::GetFighterWorld()
{
	return const_cast<IFighterWorld*>(static_cast<const AFighterCharacter*>(this)->GetFighterWorld());
}
const IFighterWorld* AFighterCharacter::GetFighterWorld() const
{
	if (GetWorld() != nullptr)
	{
		if (GetWorld()->GetAuthGameMode() != nullptr)
		{
			return Cast<IFighterWorld>((UObject*)GetWorld()->GetAuthGameMode());
		}
		else
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s world GetAuthGameMode returned null."), *GetNameSafe(this));
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s GetWorld() returned null."), *GetNameSafe(this));
		return nullptr;
	}
}

IFighterState* AFighterCharacter::GetFighterState()
{
	return const_cast<IFighterState*>(static_cast<const AFighterCharacter*>(this)->GetFighterState());
}
const IFighterState* AFighterCharacter::GetFighterState() const
{
	if (MyFighterState == nullptr)
	{
		return Cast<IFighterState>(FighterState);
	}
	else
	{
		return MyFighterState;
	}
}

IMoveset* AFighterCharacter::GetMoveset()
{
	return const_cast<IMoveset*>(static_cast<const AFighterCharacter*>(this)->GetMoveset());
}
const IMoveset* AFighterCharacter::GetMoveset() const
{
	return Cast<IMoveset>(Moveset);
}

IInputParser* AFighterCharacter::GetInputParser()
{
	return const_cast<IInputParser*>(static_cast<const AFighterCharacter*>(this)->GetInputParser());
}
const IInputParser* AFighterCharacter::GetInputParser() const
{
	return Cast<IInputParser>(InputParser);
}

ISoloTracker* AFighterCharacter::GetSoloTracker()
{
	return const_cast<ISoloTracker*>(static_cast<const AFighterCharacter*>(this)->GetSoloTracker());
}
const ISoloTracker* AFighterCharacter::GetSoloTracker() const
{
	return Cast<ISoloTracker>(SoloTracker);
}

void AFighterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bool AllGood = true;

	if (GetSelfAsFighter() == nullptr)
	{
		AllGood = false;
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s does not implement IFighter interface... what? How?"), *GetNameSafe(this));
	}
	if (GetFighterWorld() == nullptr)
	{
		AllGood = false;
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s GameMode does not implement IFighterWorld interface."), *GetNameSafe(this));
	}
	if (GetFighterState() == nullptr)
	{
		AllGood = false;
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s FighterState does not implement IFighterState interface."), *GetNameSafe(this));
	}
	if (GetMoveset() == nullptr)
	{
		AllGood = false;
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s Moveset does not implement IMoveset interface."), *GetNameSafe(this));
	}
	if (GetInputParser() == nullptr)
	{
		AllGood = false;
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s InputParser does not implement IInputParser interface."), *GetNameSafe(this));
	}
	if (GetSoloTracker() == nullptr)
	{
		AllGood = false;
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s SoloTracker does not implement ISoloTracker interface."), *GetNameSafe(this));
	}

	if (AllGood)
	{
		GetFighterState()->RegisterFighterWorld(TWeakObjectPtr<UObject>(Cast<UObject>(GetFighterWorld())));
		GetFighterState()->RegisterFighter(TWeakObjectPtr<UObject>(Cast<UObject>(this)));
		GetFighterState()->RegisterMoveset(TWeakObjectPtr<UObject>(Cast<UObject>(Moveset)));
		GetFighterState()->RegisterInputParser(TWeakObjectPtr<UObject>(Cast<UObject>(InputParser)));
		if (GetController() != nullptr && GetController()->PlayerState != nullptr)
		{
			GetFighterState()->RegisterFighterPlayerState(TWeakObjectPtr<UObject>(Cast<UObject>(GetController()->PlayerState)));
		}

		GetMoveset()->RegisterFighterWorld(TWeakObjectPtr<UObject>(Cast<UObject>(GetFighterWorld())));
		GetMoveset()->RegisterFighter(TWeakObjectPtr<UObject>(Cast<UObject>(this)));
		GetMoveset()->RegisterFighterState(TWeakObjectPtr<UObject>(Cast<UObject>(FighterState)));
		GetMoveset()->RegisterInputParser(TWeakObjectPtr<UObject>(Cast<UObject>(InputParser)));
		GetMoveset()->RegisterSoloTracker(TWeakObjectPtr<UObject>(Cast<UObject>(SoloTracker)));
		GetMoveset()->RegisterMusicBox(TWeakObjectPtr<UObject>(GetFighterWorld()->GetMusicBox()));

		GetInputParser()->RegisterFighterWorld(TWeakObjectPtr<UObject>(Cast<UObject>(GetFighterWorld())));
		GetInputParser()->RegisterFighter(TWeakObjectPtr<UObject>(Cast<UObject>(this)));
		GetInputParser()->RegisterFighterState(TWeakObjectPtr<UObject>(Cast<UObject>(FighterState)));
		GetInputParser()->RegisterMoveset(TWeakObjectPtr<UObject>(Cast<UObject>(Moveset)));

		GetSoloTracker()->RegisterFighterWorld(TWeakObjectPtr<UObject>(Cast<UObject>(GetFighterWorld())));
		GetSoloTracker()->RegisterFighter(TWeakObjectPtr<UObject>(Cast<UObject>(this)));

		GetSelfAsFighter()->RegisterFighterState(TWeakObjectPtr<UObject>(Cast<UObject>(FighterState)));
	}
}

void AFighterCharacter::RegisterFighterState(TWeakObjectPtr<UObject> NewFighterState)
{
	if (!NewFighterState.IsValid())
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s given invalid object to register as FighterState."), *GetNameSafe(this));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(NewFighterState.Get());

		if (MyFighterState == nullptr)
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("FighterCharacter %s given %s to register as FighterState, but it doesn't implement IFighter."), *GetNameSafe(this), *GetNameSafe(NewFighterState.Get()));
		}
	}
}

void AFighterCharacter::RegisterOpponent(TWeakObjectPtr<AActor> Opponent)
{
	MyOpponent = Opponent;
}

float AFighterCharacter::GetOpponentDirection() const
{
	//TODO
	if (MyOpponent == nullptr)
		return 0.f;

	FVector MyPosition = GetActorLocation();
	FVector OpponentPosition = MyOpponent->GetActorLocation();

	return FMath::Sign((OpponentPosition - MyPosition).X);
}

void AFighterCharacter::ApplyMovement(FMovement Movement)
{
	if (GetFighterState() != nullptr)
	{
		GetFighterState()->ApplyMovement(Movement);
	}
}

bool AFighterCharacter::IsBlocking() const
{
	if (GetFighterState() != nullptr)
	{
		return GetFighterState()->IsBlocking();
	}
	return false;
}

EStance AFighterCharacter::GetStance() const
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_Jump))
		{
			return EStance::SE_Jumping;
		}
		if (GetCharacterMovement()->IsCrouching())
		{
			return EStance::SE_Crouching;
		}
		else
		{
			return EStance::SE_Standing;
		}
	}
	else
	{
		return EStance::SE_Jumping;
	}
}

TWeakObjectPtr<AController> AFighterCharacter::GetFighterController() const
{
	return GetController();
}

void AFighterCharacter::SetWantsToCrouch(bool WantsToCrouch)
{
	if (WantsToCrouch)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AFighterCharacter::SetMoveDirection(float Direction)
{
	GetCharacterMovement()->AddInputVector(FVector(Direction, 0, 0));
}

void AFighterCharacter::Jump()
{
	if (!GetWorldTimerManager().IsTimerActive(TimerHandle_Jump)
		&& GetCharacterMovement()->IsMovingOnGround()
		&& !IsJumpProvidingForce())
	{
		if (StartJumpEvent.IsBound())
		{
			StartJumpEvent.Broadcast();
		}
		GetWorldTimerManager().SetTimer(
			TimerHandle_Jump,
			this,
			&AFighterCharacter::OnJumpTimer,
			JumpDelay,
			false
		);
	}
}

void AFighterCharacter::OnJumpTimer()
{
	FTransform RelativeTransform = JumpEffects.RelativeTransform * GetActorTransform();
	if (JumpEffects.ParticleSystem != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetOwner()->GetWorld(),
			JumpEffects.ParticleSystem,
			RelativeTransform
		);
	}
	if (JumpEffects.SoundCue != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			GetOwner(),
			JumpEffects.SoundCue,
			RelativeTransform.GetLocation(),
			RelativeTransform.GetRotation().Rotator()
		);
	}
	ACharacter::Jump();
}

void AFighterCharacter::InputAxisHorizontal(float amount)
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputAxisHorizontal(amount);
	}
}

void AFighterCharacter::InputAxisVertical(float amount)
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputAxisVertical(amount);
	}
}

void AFighterCharacter::InputActionUp()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionUp(true);
	}
}

void AFighterCharacter::InputActionLeft()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionLeft(true);
	}
}

void AFighterCharacter::InputActionDown()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionDown(true);
	}
}

void AFighterCharacter::InputActionRight()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionRight(true);
	}
}

void AFighterCharacter::InputActionLight()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionLight(true);
	}
}

void AFighterCharacter::InputActionMedium()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionMedium(true);
	}
}

void AFighterCharacter::InputActionHeavy()
{
	if (GetInputParser() != nullptr)
	{
		GetInputParser()->InputActionHeavy(true);
	}
}

void AFighterCharacter::Landed(const FHitResult& Result)
{
	ACharacter::Landed(Result);

	if (MyFighterState != nullptr)
	{
		MyFighterState->OnLand();
	}
	if (LandEvent.IsBound())
	{
		LandEvent.Broadcast();
	}
}

void AFighterCharacter::SetFacing(float Sign)
{
	Sign = FMath::Sign(Sign);
	if (Sign != 0.f && Sign != Facing)
	{
		Facing = Sign;
		SetActorRotation(
			FQuat(FVector(0, 0, 1), (Sign == 1.f) ? 0.f : PI)
		);
	}
}

float AFighterCharacter::GetFacing() const
{
	return Facing;
}

bool AFighterCharacter::K2_IsBlocking() const
{
	return IsBlocking();
}

FStartJumpEvent& AFighterCharacter::GetOnStartJumpEvent()
{
	return StartJumpEvent;
}

FLandEvent& AFighterCharacter::GetOnLandEvent()
{
	return LandEvent;
}

void AFighterCharacter::SetOpponent(TWeakObjectPtr<AActor> NewOpponent)
{
	if (NewOpponent.IsValid())
	{
		UE_LOG(LogAFighterCharacter, Verbose, TEXT("%s setting opponent to %s"), *GetNameSafe(this), *GetNameSafe(NewOpponent.Get()));
		MyOpponent = NewOpponent;
	}
}

float AFighterCharacter::GetHorizontalMovement() const
{
	if (GetFighterState() != nullptr)
	{
		return GetFighterState()->GetCurrentHorizontalMovement();
	}
	return 0.f;
}

void AFighterCharacter::PossessedBy(AController *NewController)
{
	ACharacter::PossessedBy(NewController);

	if (GetFighterState() != nullptr)
	{
		GetFighterState()->RegisterFighterPlayerState(TWeakObjectPtr<UObject>(Cast<UObject>(NewController->PlayerState)));
	}
}

void AFighterCharacter::OnInputReceived()
{
	K2_OnInputReceived();
}