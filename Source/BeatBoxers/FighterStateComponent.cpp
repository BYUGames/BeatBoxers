// copyright 2017 BYU Animation

#include "FighterStateComponent.h"


// Sets default values for this component's properties
UFighterStateComponent::UFighterStateComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);
	
	ActorsToIgnore = TArray<TWeakObjectPtr<AActor>>();
	IsWindowActive = false;
}


// Called when the game starts
void UFighterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFighterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PerformHitboxScan();
}

void UFighterStateComponent::RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld)
{
	if (!FighterWorld.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given invalid object to register as FighterWorld."), *GetNameSafe(this));
	}
	else
	{
		MyFighterWorld = Cast<IFighterWorld>(FighterWorld.Get());
		if (MyFighterWorld == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as FighterWorld, but it doesn't implement IFighterWorld."), *GetNameSafe(this), *GetNameSafe(FighterWorld.Get()));
		}
	}
}

void UFighterStateComponent::RegisterFighter(TWeakObjectPtr<UObject> Fighter)
{
	if (!Fighter.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given invalid object to register as Fighter."), *GetNameSafe(this));
	}
	else
	{
		MyFighter = Cast<IFighter>(Fighter.Get());
		if (MyFighter == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as Fighter, but it doesn't implement IFighter."), *GetNameSafe(this), *GetNameSafe(Fighter.Get()));
		}
	}
}

void UFighterStateComponent::RegisterMoveset(TWeakObjectPtr<UObject> Moveset)
{
	if (!Moveset.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given invalid Object to register as Moveset."), *GetNameSafe(this));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset.Get());
		if (MyMoveset == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(this), *GetNameSafe(Moveset.Get()));
		}
	}
}

void UFighterStateComponent::RegisterInputParser(TWeakObjectPtr<UObject> InputParser)
{
	if (!InputParser.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given invalid object to register as InputParser."), *GetNameSafe(this));
	}
	else
	{
		MyInputParser = Cast<IInputParser>(InputParser.Get());
		if (MyInputParser == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as InputParser, but it doesn't implement IInputParser."), *GetNameSafe(this), *GetNameSafe(InputParser.Get()));
		}
	}
}

bool UFighterStateComponent::IsInputBlocked() const
{
	return IsMidMove() || IsStunned();
}

bool UFighterStateComponent::IsBlocking() const
{
	if (IsInputBlocked() || MoveDirection == 0 || MyFighter == nullptr) return false;
	if (MyFighter->GetStance() == EStance::SE_Jumping || MyFighter->GetStance() == EStance::SE_NA) return false;

	float ToOpponent = MyFighter->GetOpponentDirection();
	if (ToOpponent == 0) return false;

	if (ToOpponent > 0 && MoveDirection < 0) return true;
	if (ToOpponent < 0 && MoveDirection > 0) return true;

	return false;
}

bool UFighterStateComponent::IsStunned() const
{
	return GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Stun);
}

bool UFighterStateComponent::IsMidMove() const
{
	return IsWindowActive;
}

void UFighterStateComponent::StartMoveWindow(FMoveWindow& Window, bool IsLastInSequence)
{
	CurrentWindow = Window;
	IsLastWindow = IsLastInSequence;

	if (IsStunned())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterStateComponent %s was requested to start a new window despite being stunned."), *GetNameSafe(this));
	}
	else
	{
		StartCurrentWindowWindup();
	}
}

void UFighterStateComponent::StartStun(float Duration)
{
	InterruptWindow();
	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_Stun,
		this,
		&UFighterStateComponent::OnStunFinished,
		Duration,
		false
	);
}

void UFighterStateComponent::SetMoveDirection(float Direction)
{
	if (!IsInputBlocked() && MyFighter != nullptr)
	{
		MoveDirection = Direction;
		MyFighter->SetMoveDirection(Direction);
	}
	MoveDirection = 0;
}

void UFighterStateComponent::SetWantsToCrouch(bool WantsToCrouch)
{
	if (IsInputBlocked() || MyFighter == nullptr) return;
	if (MyFighter->GetStance() == EStance::SE_Jumping || MyFighter->GetStance() == EStance::SE_NA)
	{
		MyFighter->SetWantsToCrouch(false);
		return;
	}
	MyFighter->SetWantsToCrouch(WantsToCrouch);
}

void UFighterStateComponent::Jump()
{
	if (IsInputBlocked() || MyFighter == nullptr) return;

	MyFighter->Jump();
}

void UFighterStateComponent::OnLand()
{
	if (IsWindowActive)
	{
		if (CurrentWindow.LandingInterrupts)
		{
			InterruptWindow();
		}
		else if (CurrentWindow.LandingEndsWindow)
		{
			if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Window))
			{
				GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Window);
			}
			if (MyMoveset != nullptr)
			{
				MyMoveset->OnWindowFinished(false);
			}
		}
	}
}

void UFighterStateComponent::StartCurrentWindowWindup()
{
	IsWindowActive = true;
	ActorsToIgnore.Empty();
	ActorsToIgnore.Add(GetOwner());

	if (CurrentWindow.Windup <= 0)
	{
		// No windup, proceed.
		StartCurrentWindow();
	}
	else
	{
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_Window,
			this,
			&UFighterStateComponent::OnCurrentWindowWindupFinished,
			CurrentWindow.Windup,
			false
		);
	}
}

void UFighterStateComponent::OnCurrentWindowWindupFinished()
{
	StartCurrentWindow();
}

void UFighterStateComponent::StartCurrentWindow()
{
	if (CurrentWindow.Duration <= 0)
	{
		// Window has no duration.
		if (CurrentWindow.IsHitboxActive)
		{
			PerformHitboxScan();
		}
		OnCurrentWindowFinished();
	}
	else
	{
		if (CurrentWindow.IsHitboxActive)
		{
			SetComponentTickEnabled(true);
		}
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_Window,
			this,
			&UFighterStateComponent::OnCurrentWindowFinished,
			CurrentWindow.Duration,
			false
		);
	}
}

void UFighterStateComponent::OnCurrentWindowFinished()
{
	SetComponentTickEnabled(false);
	StartCurrentWindowWinddown();
}

void UFighterStateComponent::StartCurrentWindowWinddown()
{
	if (CurrentWindow.Winddown <= 0)
	{
		// Window has no winddown.
		OnCurrentWindowWinddownFinished();
	}
	else
	{
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_Window,
			this,
			&UFighterStateComponent::OnCurrentWindowWinddownFinished,
			CurrentWindow.Winddown,
			false
		);
	}
}

void UFighterStateComponent::OnCurrentWindowWinddownFinished()
{
	IsWindowActive = false;
	if (MyMoveset != nullptr)
		MyMoveset->OnWindowFinished(false);
	if (IsLastWindow && MyInputParser != nullptr)
	{
		MyInputParser->OnControlReturned();
	}
}

void UFighterStateComponent::InterruptWindow()
{
	IsWindowActive = false;
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Window))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Window);
		if (MyMoveset != nullptr)
		{
			MyMoveset->OnWindowFinished(true);
		}
	}
}

void UFighterStateComponent::PerformHitboxScan()
{
	if (!IsWindowActive)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterStateComponent tick is active but current window is not."), *GetNameSafe(this));
		SetComponentTickEnabled(false);
		return;
	}

	if (!CurrentWindow.IsHitboxActive)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("FighterStateComponent tick is active but current window's hitbox is not active."), *GetNameSafe(this));
		SetComponentTickEnabled(false);
		return;
	}

	if (MyFighterWorld != nullptr)
	{
		FHitResult HitResult = MyFighterWorld->TraceHitbox(
			CurrentWindow.Hitbox,
			ActorsToIgnore
		);

		if (HitResult.bBlockingHit)
		{
			if (HitResult.Actor.IsValid())
			{
				APawn *Pawn = Cast<APawn>(GetOwner());
				ActorsToIgnore.Add(HitResult.Actor);
				if (MyFighter != nullptr)
				{
					MyFighterWorld->HitActor(
						HitResult.Actor,
						MyFighterWorld->GetDamageType(
							MyFighter->GetStance(),
							CurrentWindow.DamageTypeOverride
						),
						CurrentWindow.DefenderHit,
						CurrentWindow.DefenderBlock,
						GetOwner(),
						(Pawn == nullptr) ? nullptr : Pawn->GetController()
					);
				}
			}
		}
	}
}

void UFighterStateComponent::OnStunFinished()
{
	if (MyInputParser != nullptr)
	{
		MyInputParser->OnControlReturned();
	}
}