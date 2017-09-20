// copyright 2017 BYU Animation

#include "FighterCharacter.h"


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

	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 1, 0));
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

}

// Called to bind functionality to input
void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
			UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s world GetAuthGameMode returned null."), *GetNameSafe(this));
			return nullptr;
		}
	}
	else
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s GetWorld() returned null."), *GetNameSafe(this));
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
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s does not implement IFighter interface... what? How?"), *GetNameSafe(this));
	}
	if (GetFighterWorld() == nullptr)
	{
		AllGood = false;
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s GameMode does not implement IFighterWorld interface."), *GetNameSafe(this));
	}
	if (GetFighterState() == nullptr)
	{
		AllGood = false;
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s FighterState does not implement IFighterState interface."), *GetNameSafe(this));
	}
	if (GetMoveset() == nullptr)
	{
		AllGood = false;
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s Moveset does not implement IMoveset interface."), *GetNameSafe(this));
	}
	if (GetInputParser() == nullptr)
	{
		AllGood = false;
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s InputParser does not implement IInputParser interface."), *GetNameSafe(this));
	}
	if (GetSoloTracker() == nullptr)
	{
		AllGood = false;
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s SoloTracker does not implement ISoloTracker interface."), *GetNameSafe(this));
	}

	if (AllGood)
	{
		GetSelfAsFighter()->RegisterFighterState(TWeakObjectPtr<UObject>(Cast<UObject>(FighterState)));

		GetFighterState()->RegisterFighterWorld(TWeakObjectPtr<UObject>(Cast<UObject>(GetFighterWorld())));
		GetFighterState()->RegisterFighter(TWeakObjectPtr<UObject>(Cast<UObject>(this)));
		GetFighterState()->RegisterMoveset(TWeakObjectPtr<UObject>(Cast<UObject>(Moveset)));
		GetFighterState()->RegisterInputParser(TWeakObjectPtr<UObject>(Cast<UObject>(InputParser)));

		GetMoveset()->RegisterFighterState(TWeakObjectPtr<UObject>(Cast<UObject>(FighterState)));
		GetMoveset()->RegisterSoloTracker(TWeakObjectPtr<UObject>(Cast<UObject>(SoloTracker)));

		GetSoloTracker()->RegisterMoveset(TWeakObjectPtr<UObject>(Cast<UObject>(Moveset)));

		GetInputParser()->RegisterFighterState(TWeakObjectPtr<UObject>(Cast<UObject>(FighterState)));
		GetInputParser()->RegisterMoveset(TWeakObjectPtr<UObject>(Cast<UObject>(Moveset)));
	}
}

void AFighterCharacter::RegisterFighterState(TWeakObjectPtr<UObject> NewFighterState)
{
	if (NewFighterState.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s given invalid object to register as FighterState."), *GetNameSafe(this));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(NewFighterState.Get());

		if (MyFighterState == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("FighterCharacter %s given %s to register as FighterState, but it doesn't implement IFighter."), *GetNameSafe(this), *GetNameSafe(NewFighterState.Get()));
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
	return 0.f;
}

void AFighterCharacter::ApplyMovement(FMovement Delta)
{
	//TODO
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
	//TODO
	return EStance::SE_Standing;
}

TWeakObjectPtr<AController> AFighterCharacter::GetFighterController() const
{
	return GetController();
}

void AFighterCharacter::SetWantsToCrouch(bool WantsToCrouch)
{
	//TODO
}

void AFighterCharacter::SetMoveDirection(float Direction)
{
	//TODO
}

void AFighterCharacter::Jump()
{
	//TODO
}
