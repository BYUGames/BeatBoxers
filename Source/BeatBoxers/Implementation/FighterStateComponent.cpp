// copyright 2017 BYU Animation

#include "FighterStateComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "FighterCharacter.h"


// Sets default values for this component's properties
UFighterStateComponent::UFighterStateComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	Special = 0.f;
	ActorsToIgnore = TArray<TWeakObjectPtr<AActor>>();
	IsWindowActive = false;
	IsHitboxActive = false;
}


// Called when the game starts
void UFighterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
	
	AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
	if (Fighter != nullptr)
	{
		DefaultHitEffects = Fighter->DefaultHitEffects;
		DefaultBlockEffects = Fighter->DefaultBlockEffects;
	}
}


// Called every frame
void UFighterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool action = false;
	if (IsWindowActive && CurrentWindow.IsHitboxActive && IsHitboxActive)
	{
		action = true;
		PerformHitboxScan();
	}
	// Don't swap the order or short circuit evaluation might result in the call not being made
	action = MovementStep(DeltaTime) || action;
	if (!action)
	{
		SetComponentTickEnabled(false);
	}
}

bool UFighterStateComponent::MovementStep(float DeltaTime)
{
	FVector LastSpot = GetOwner()->GetActorLocation();

	if (IsBeingMoved)
	{
		FVector TargetLocation = GetOwner()->GetActorLocation();
		if (CurrentMovement.UseDeltaAsSpeed)
		{
			TargetLocation += CurrentMovement.Delta * DeltaTime;
		}
		else
		{
			TargetLocation += CurrentMovement.Delta / CurrentMovement.Duration * DeltaTime;
		}
		GetOwner()->SetActorLocation(
			GetOwner()->GetActorLocation() + CurrentMovement.Delta / CurrentMovement.Duration * DeltaTime,
			true,
			nullptr,
			ETeleportType::TeleportPhysics
		);
		return true;
	}
	return false;
}

void UFighterStateComponent::RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld)
{
	if (!FighterWorld.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given invalid object to register as FighterWorld."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterWorld = Cast<IFighterWorld>(FighterWorld.Get());
		if (MyFighterWorld == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given %s to register as FighterWorld, but it doesn't implement IFighterWorld."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterWorld.Get()));
		}
	}
}

void UFighterStateComponent::RegisterFighter(TWeakObjectPtr<UObject> Fighter)
{
	if (!Fighter.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given invalid object to register as Fighter."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighter = Cast<IFighter>(Fighter.Get());
		if (MyFighter == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given %s to register as Fighter, but it doesn't implement IFighter."), *GetNameSafe(GetOwner()), *GetNameSafe(Fighter.Get()));
		}
	}
}

void UFighterStateComponent::RegisterMoveset(TWeakObjectPtr<UObject> Moveset)
{
	if (!Moveset.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given invalid Object to register as Moveset."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset.Get());
		if (MyMoveset == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(GetOwner()), *GetNameSafe(Moveset.Get()));
		}
	}
}

void UFighterStateComponent::RegisterInputParser(TWeakObjectPtr<UObject> InputParser)
{
	if (!InputParser.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given invalid object to register as InputParser."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyInputParser = Cast<IInputParser>(InputParser.Get());
		if (MyInputParser == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given %s to register as InputParser, but it doesn't implement IInputParser."), *GetNameSafe(GetOwner()), *GetNameSafe(InputParser.Get()));
		}
	}
}

bool UFighterStateComponent::IsInputBlocked() const
{
	return IsMidMove() || IsStunned();
}

bool UFighterStateComponent::IsBlocking() const
{
	float ToOpponent = (MyFighter != nullptr) ? MyFighter->GetOpponentDirection() : 0.f;
	UE_LOG(LogUFighterState, VeryVerbose, TEXT("%s UFighterStateComponent IsBlocking %d, %d, %d, %f, %s, %s, %f"),
		*GetNameSafe(GetOwner()),
		IsStunned(),
		IsCurrentStunBlock,
		IsInputBlocked(),
		MoveDirection,
		(MyFighter != nullptr) ? TEXT("Valid") : TEXT("nullptr"),
		*GetEnumValueToString<EStance>(TEXT("EStance"), (MyFighter != nullptr) ? MyFighter->GetStance() : EStance::SE_NA),
		ToOpponent
	);
	if (IsStunned() && IsCurrentStunBlock) return true;
	if (IsInputBlocked() || MoveDirection == 0 || MyFighter == nullptr) return false;
	if (MyFighter->GetStance() == EStance::SE_Jumping || MyFighter->GetStance() == EStance::SE_NA) return false;

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

void UFighterStateComponent::StartMoveWindow(FMoveWindow& Window)
{
	UE_LOG(LogUFighterState, Verbose, TEXT("%s UFighterStateComponent starting new move window."), *GetNameSafe(GetOwner()));
	CurrentWindow = Window;
	if (CurrentWindow.IsHitboxActive)
	{
		if (!CurrentWindow.DefenderHit.SFX.IsValid())
		{
			CurrentWindow.DefenderHit.SFX = DefaultHitEffects;
		}
		if (!CurrentWindow.DefenderBlock.SFX.IsValid())
		{
			CurrentWindow.DefenderBlock.SFX = DefaultBlockEffects;
		}
	}
	CurrentWindowEnd = EWindowEnd::WE_Finished;

	if (IsStunned())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent was requested to start a new window despite being stunned."), *GetNameSafe(GetOwner()));
	}
	else
	{
		if (Window.AnimMontage != nullptr && MyFighter != nullptr)
		{
			ACharacter *Character = Cast<ACharacter>(MyFighter);
			if (Character != nullptr)
			{
				float Duration = Character->PlayAnimMontage(Window.AnimMontage);
				if (Duration == 0.f)
				{
					UE_LOG(LogBBAnimation, Warning, TEXT("%s::StartMoveWindow PlayAnimMontage(%s) returned 0 as duration."), *GetNameSafe(this), *GetNameSafe(Window.AnimMontage));
				}
				else
				{
					UE_LOG(LogBBAnimation, Verbose, TEXT("%s::StartMoveWindow PlayAnimMontage(%s) duration %f."), *GetNameSafe(this), *GetNameSafe(Window.AnimMontage), Duration);
				}
			}
			else
			{
				UE_LOG(LogBBAnimation, Warning, TEXT("%s::StartMoveWindow unable to cast fighter to character."), *GetNameSafe(this));
			}
		}
		StartCurrentWindowWindup();
	}
}

void UFighterStateComponent::StartStun(float Duration, bool WasBlocked)
{
	IsCurrentStunBlock = WasBlocked;
	if (IsWindowActive && CurrentWindow.AnimMontage != nullptr && MyFighter != nullptr)
	{
		ACharacter *Character = Cast<ACharacter>(MyFighter);
		if (Character != nullptr)
		{
			Character->StopAnimMontage();
		}
	}
	EndWindow(EWindowEnd::WE_Stunned);
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

void UFighterStateComponent::ApplyMovement(FMovement Movement)
{
	if (!Movement.IsValid())
	{
		UE_LOG(LogUFighterState, Warning, TEXT("%s UFighterStateComponent asked to apply invalid movement."), *GetNameSafe(GetOwner()));
		return;
	}

	UE_LOG(LogUFighterState, Verbose, TEXT("%s UFighterStateComponent at %s ApplyMovement(%s)"), *GetNameSafe(GetOwner()), *GetOwner()->GetActorLocation().ToString(), *Movement.ToString());

	IsBeingMoved = true;
	CurrentMovement = Movement;
	if (Movement.Duration == 0)
	{
		MovementStep(1.f);
		IsBeingMoved = false;
	}
	else
	{
		SetComponentTickEnabled(true);
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_Movement,
			this,
			&UFighterStateComponent::OnMovementTimer,
			CurrentMovement.Duration,
			false
		);
	}
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
			OnMovementTimer();
			CurrentWindowEnd = EWindowEnd::WE_LandInt;
			OnCurrentWindowFinished();
		}
		else if (CurrentWindow.LandingEndsWindow)
		{
			OnMovementTimer();
			CurrentWindowEnd = EWindowEnd::WE_LandSkip;
			OnCurrentWindowFinished();
		}
		else
		{
			MyFighterWorld->AdjustLocation(GetOwner());
		}
	}
	else
	{
		MyFighterWorld->AdjustLocation(GetOwner());
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
	if (CurrentWindow.AttackerMovement)
	{
		if (MyFighterWorld != nullptr)
		{
			MyFighterWorld->ApplyMovementToActor(GetOwner(), GetOwner(), GetOwnerController(), CurrentWindow.AttackerMovement);
		}
	}
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
			IsHitboxActive = true;
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
	IsHitboxActive = false;
	TryDisableTick();
	StartCurrentWindowWinddown();
}

void UFighterStateComponent::StartCurrentWindowWinddown()
{
	HasMoveWindowHit = false;
	PlayerAttackerEffects(CurrentWindow.SFX);
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
	if (HasMoveWindowHit == false && CurrentWindowEnd == EWindowEnd::WE_Finished)
	{
		PlayerAttackerEffects(CurrentWindow.MissSFX);
	}
	//Uses CurrentWindowEnd so landing interrutps can still play winddown.
	EndWindow(CurrentWindowEnd);
}

void UFighterStateComponent::EndWindow(EWindowEnd WindowEnd)
{
	if (IsWindowActive)
	{
		IsWindowActive = false;
		if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Window))
		{
			GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Window);
		}
		if (MyMoveset != nullptr)
		{
			MyMoveset->OnWindowFinished(WindowEnd);
		}
	}
}

void UFighterStateComponent::PerformHitboxScan()
{

	if (MyFighterWorld != nullptr)
	{
		FMoveHitbox WorldHitbox;
		WorldHitbox.Radius = CurrentWindow.Hitbox.Radius;
		FTransform Transform = GetOwner()->GetActorTransform();
		WorldHitbox.Origin = Transform.TransformPositionNoScale(CurrentWindow.Hitbox.Origin);
		WorldHitbox.End = Transform.TransformPositionNoScale(CurrentWindow.Hitbox.End);

		FHitResult HitResult = MyFighterWorld->TraceHitbox(
			WorldHitbox,
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
					EHitResponse Response = MyFighterWorld->HitActor(
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

					FTransform ImpactTransform = FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector::OneVector);
					FTransform RelativeTransform;
					switch (Response)
					{
					case EHitResponse::HE_Hit:
						HasMoveWindowHit = true;
						RelativeTransform = CurrentWindow.DefenderHit.SFX.RelativeTransform * ImpactTransform;
						if (CurrentWindow.DefenderHit.SFX.ParticleSystem != nullptr)
						{
							UGameplayStatics::SpawnEmitterAtLocation(
								GetOwner()->GetWorld(),
								CurrentWindow.DefenderHit.SFX.ParticleSystem,
								RelativeTransform
							);
						}
						if (CurrentWindow.DefenderHit.SFX.SoundCue != nullptr)
						{
							UGameplayStatics::SpawnSoundAtLocation(
								GetOwner(),
								CurrentWindow.DefenderHit.SFX.SoundCue,
								RelativeTransform.GetLocation(),
								RelativeTransform.GetRotation().Rotator()
							);
						}
						break;
					case EHitResponse::HE_Blocked:
						HasMoveWindowHit = true;
						RelativeTransform = CurrentWindow.DefenderBlock.SFX.RelativeTransform * ImpactTransform;
						if (CurrentWindow.DefenderBlock.SFX.ParticleSystem != nullptr)
						{
							UGameplayStatics::SpawnEmitterAtLocation(
								GetOwner()->GetWorld(),
								CurrentWindow.DefenderBlock.SFX.ParticleSystem,
								RelativeTransform
							);
						}
						if (CurrentWindow.DefenderBlock.SFX.SoundCue != nullptr)
						{
							UGameplayStatics::SpawnSoundAtLocation(
								GetOwner(),
								CurrentWindow.DefenderBlock.SFX.SoundCue,
								RelativeTransform.GetLocation(),
								RelativeTransform.GetRotation().Rotator()
							);
						}
						break;
					default:
						break;
					}
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

void UFighterStateComponent::PlayerAttackerEffects(FEffects& Effects)
{
	if (Effects.AttachToActor)
	{
		if (Effects.ParticleSystem != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(
				Effects.ParticleSystem,
				GetOwner()->GetDefaultAttachComponent(),
				NAME_None,
				Effects.RelativeTransform.GetTranslation(),
				Effects.RelativeTransform.GetRotation().Rotator()
				);
		}
		if (Effects.SoundCue != nullptr)
		{
			UGameplayStatics::SpawnSoundAttached(
				Effects.SoundCue,
				GetOwner()->GetDefaultAttachComponent(),
				NAME_None,
				Effects.RelativeTransform.GetTranslation(),
				Effects.RelativeTransform.GetRotation().Rotator()
			);
		}
	}
	else
	{
		FTransform RelativeTransform = Effects.RelativeTransform * GetOwner()->GetActorTransform();
		UE_LOG(LogUFighterState, Verbose, TEXT("Owner location %s, spawn location %s"), *GetOwner()->GetActorLocation().ToString(), *RelativeTransform.GetLocation().ToString());
		if (Effects.ParticleSystem != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetOwner()->GetWorld(),
				Effects.ParticleSystem,
				RelativeTransform
			);
		}
		if (Effects.SoundCue != nullptr)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				GetOwner(),
				Effects.SoundCue,
				RelativeTransform.GetLocation(),
				RelativeTransform.GetRotation().Rotator()
			);
		}
	}
}

void UFighterStateComponent::TryDisableTick()
{
	if ((!IsWindowActive || !CurrentWindow.IsHitboxActive || !IsHitboxActive) && !IsBeingMoved)
	{
		SetComponentTickEnabled(false);
	}
}

void UFighterStateComponent::OnMovementTimer()
{
	IsBeingMoved = false;
	TryDisableTick();
	MyFighterWorld->AdjustLocation(GetOwner());
}

AController* UFighterStateComponent::GetOwnerController() const
{
	APawn *PawnOwner = Cast<APawn>(GetOwner());
	if (PawnOwner != nullptr) return PawnOwner->Controller;
	return nullptr;
}

float UFighterStateComponent::GetSpecialAmount() const
{
	return Special;
}

void UFighterStateComponent::AddSpecial(float Amount)
{
	Special += Amount;
}

bool UFighterStateComponent::UseSpecial(float Amount)
{
	if (Special >= Amount)
	{
		Special -= Amount;
		return true;
	}
	return false;
}

EStance UFighterStateComponent::GetStance() const
{
	if (MyFighter == nullptr)
	{
		return EStance::SE_NA;
	}
	return MyFighter->GetStance();
}

TSubclassOf<AMoveState> UFighterStateComponent::GetDefaultMoveState()
{
	if (MyFighter == nullptr)
	{
		return TSubclassOf<AMoveState>(AMoveState::StaticClass());
	}
	return MyFighter->GetDefaultMoveState();
}

float UFighterStateComponent::GetCurrentHorizontalMovement() const
{
	if (IsBeingMoved)
	{
		if (CurrentMovement.UseDeltaAsSpeed)
		{
			return CurrentMovement.Delta.X;
		}
		return CurrentMovement.Delta.X / CurrentMovement.Duration;
	}
	return 0.f;
}