// copyright 2017 BYU Animation

#include "FighterStateComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "FighterCharacter.h"
#include "../../Interfaces/IMusicBox.h"


// Sets default values for this component's properties
UFighterStateComponent::UFighterStateComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	ActorsToIgnore = TArray<TWeakObjectPtr<AActor>>();
	CurrentWindowStage = EWindowStage::WE_None;
	bIsHitboxActive = false;
	TimesHitThisKnockdown = 0;
	bIsInDDR = false;
}


// Called when the game starts
void UFighterStateComponent::BeginPlay()
{
	Super::BeginPlay();
	cs = GetNameSafe(this);

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
	if (IsMidMove() && CurrentWindow.IsHitboxActive && bIsHitboxActive)
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

	if (bIsBeingMoved)
	{
		FVector TargetLocation = GetOwner()->GetActorLocation();
		FVector Movement = FVector(CurrentMovement.Delta.X, 0, CurrentMovement.Delta.Y);
		if (CurrentMovement.UseDeltaAsSpeed)
		{
			TargetLocation += Movement * DeltaTime;
		}
		else
		{
			TargetLocation += Movement / CurrentMovement.Duration * DeltaTime;
		}
		GetOwner()->SetActorLocation(
			TargetLocation,
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
	if (MyFighterWorld != nullptr)
	{
		MyFighterWorld->GetOnSoloStartEvent().RemoveDynamic(this, &UFighterStateComponent::OnSoloStart);
		MyFighterWorld->GetOnSoloEndEvent().RemoveDynamic(this, &UFighterStateComponent::OnSoloEnd);
		MyFighterWorld->GetOnBeatWindowCloseEvent().RemoveDynamic(this, &UFighterStateComponent::SkipWindup);
		if (MyFighterWorld->GetMusicBox() != nullptr
			&& Cast<IMusicBox>(MyFighterWorld->GetMusicBox()) != nullptr)
		{
			auto MusicBox = Cast<IMusicBox>(MyFighterWorld->GetMusicBox());
			MusicBox->GetOnBeatEvent().RemoveDynamic(this, &UFighterStateComponent::SkipWindupOnBeat);
		}
	}
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

		MyFighterWorld->GetOnSoloStartEvent().AddDynamic(this, &UFighterStateComponent::OnSoloStart);
		MyFighterWorld->GetOnSoloEndEvent().AddDynamic(this, &UFighterStateComponent::OnSoloEnd);
		MyFighterWorld->GetOnBeatWindowCloseEvent().AddDynamic(this, &UFighterStateComponent::SkipWindup);
		if (MyFighterWorld->GetMusicBox() != nullptr
			&& Cast<IMusicBox>(MyFighterWorld->GetMusicBox()) != nullptr)
		{
			auto MusicBox = Cast<IMusicBox>(MyFighterWorld->GetMusicBox());
			MusicBox->GetOnBeatEvent().AddDynamic(this, &UFighterStateComponent::SkipWindupOnBeat);
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

void UFighterStateComponent::RegisterFighterPlayerState(TWeakObjectPtr<UObject> FighterPlayerState)
{
	if (!FighterPlayerState.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given invalid object to register as FighterPlayerState."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterPlayerState = Cast<IFighterPlayerState>(FighterPlayerState.Get());
		if (MyFighterPlayerState == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent given %s to register as FighterPlayerState, but it doesn't implement IFighterPlayerState."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterPlayerState.Get()));
		}
	}
}

bool UFighterStateComponent::IsInputBlocked() const
{
	return IsMidMove() || IsStunned() || bIsFrozenForSolo;
}

bool UFighterStateComponent::IsBlocking() const
{
	float ToOpponent = (MyFighter != nullptr) ? MyFighter->GetOpponentDirection() : 0.f;
	UE_LOG(LogUFighterState, VeryVerbose, TEXT("%s UFighterStateComponent IsBlocking %d, %d, %d, %d, %f, %s, %s, %f"),
		*GetNameSafe(GetOwner()),
		IsStunned(),
		bIsCurrentStunBlock,
		bIsBlockButtonDown,
		IsInputBlocked(),
		MoveDirection,
		(MyFighter != nullptr) ? TEXT("Valid") : TEXT("nullptr"),
		*GetEnumValueToString<EStance>(TEXT("EStance"), (MyFighter != nullptr) ? MyFighter->GetStance() : EStance::SE_NA),
		ToOpponent
	);
	if (!IsInputBlocked() && bIsBlockButtonDown) return true;
	if (IsStunned() && bIsCurrentStunBlock) return true;
	if (IsInputBlocked() || MoveDirection == 0 || MyFighter == nullptr) return false;
	if (MyFighter->GetStance() == EStance::SE_Jumping || MyFighter->GetStance() == EStance::SE_NA) return false;

	if (ToOpponent == 0) return false;

	//if (ToOpponent > 0 && MoveDirection < 0) return true;
	//if (ToOpponent < 0 && MoveDirection > 0) return true;

	return false;
}

bool UFighterStateComponent::IsCharging() const
{
	return !IsInputBlocked() && MyFighter->GetStance() == EStance::SE_Standing && bWantsToCharge;
}

bool UFighterStateComponent::IsStunned() const
{
	return GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Stun) || IsKnockedDown() || IsGrabbed();
}

bool UFighterStateComponent::IsGrabbed() const
{
	return bGrabbed;
}

bool UFighterStateComponent::IsBlockStunned() const
{
	return GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Stun) && bIsCurrentStunBlock;
}

bool UFighterStateComponent::IsKnockedDown() const
{
	return bIsKnockedDown;
}

bool UFighterStateComponent::IsInvulnerable() const
{
	return GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Invulnerable) || (CurrentWindow.Invincibility && IsMidMove());
}

bool UFighterStateComponent::IsMidMove() const
{
	return CurrentWindowStage != EWindowStage::WE_None;
}

void UFighterStateComponent::StartMoveWindow(FMoveWindow& Window, float Accuracy)
{
	UE_LOG(LogUFighterState, Verbose, TEXT("%s UFighterStateComponent starting new move window."), *GetNameSafe(GetOwner()));
	UE_LOG(LogBeatTiming, VeryVerbose, TEXT("%s UFighterStateComponent starting new move window with accuracy %f."), *GetNameSafe(GetOwner()), Accuracy);
	CurrentWindow = Window;
	CurrentWindowAccuracy = Accuracy;
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
	CurrentWindowEnd = EWindowEnd::WE_Missed;

	if (IsStunned())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UFighterStateComponent was requested to start a new window despite being stunned."), *GetNameSafe(GetOwner()));
	}
	else
	{
		StartCurrentWindowWindup();
	}
}

bool UFighterStateComponent::IsInCrouchMove()
{
	return CurrentWindow.CrouchAttack && IsMidMove();
}

void UFighterStateComponent::StartStun(float Duration, bool WasBlocked)
{
	bIsCurrentStunBlock = WasBlocked;
	StartDDR();
	if (IsMidMove() && CurrentMontage != nullptr && MyFighter != nullptr)
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
	else {
		MoveDirection = 0;
	}
}

void UFighterStateComponent::SetVerticalDirection(float Direction)
{
	VerticalDirection = Direction;
}

void UFighterStateComponent::SetHorizontalDirection(float Direction)
{
	HorizontalDirection = Direction;
}

void UFighterStateComponent::SetWantsToCrouch(bool WantsToCrouch)
{
	if (IsInputBlocked() || MyFighter == nullptr) return;
	if (MyFighter->GetStance() == EStance::SE_Jumping || MyFighter->GetStance() == EStance::SE_NA || MyFighter->IsJumping())
	{
		MyFighter->SetWantsToCrouch(false);
		return;
	}
	if (WantsToCrouch && MyFighter->GetStance() != EStance::SE_Crouching)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("In FighterStateComp SetWantsToCrouch"));
	}
	MyFighter->SetWantsToCrouch(WantsToCrouch);
}

void UFighterStateComponent::SetWantsToCharge(bool WantsToCharge)
{
	bWantsToCharge = WantsToCharge;
}

void UFighterStateComponent::ApplyMovement(FMovement Movement)
{
	if (!Movement.IsValid())
	{
		UE_LOG(LogUFighterState, Verbose, TEXT("%s UFighterStateComponent stopping movement."), *GetNameSafe(GetOwner()));
		bIsBeingMoved = false;
		TryDisableTick();
		return;
	}

	UE_LOG(LogUFighterState, Verbose, TEXT("%s UFighterStateComponent at %s ApplyMovement(%s)"), *GetNameSafe(GetOwner()), *GetOwner()->GetActorLocation().ToString(), *Movement.ToString());

	bIsBeingMoved = true;
	CurrentMovement = Movement;
	if (Movement.Delta.Y > 0.f)
	{
		UCharacterMovementComponent *MovementComponent = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
		if (MovementComponent != nullptr && MovementComponent->IsMovingOnGround())
		{
			UE_LOG(LogUFighterState, Verbose, TEXT("%s UFighterStateComponent ApplyMovement set falling."), *GetNameSafe(GetOwner()));
			// This is actually redundant since the gamemode applies an effectively zero launch to the character prior to calling this.
			// That call sets the move mode to falling.
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}
	if (Movement.Duration == 0)
	{
		MovementStep(1.f);
		bIsBeingMoved = false;
	}
	else
	{
		Cast<AFighterCharacter>(MyFighter)->AirMovementDirection = 0;
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
	if (IsInputBlocked() || MyFighter == nullptr || MyFighter->IsJumping()) return;
	SetWantsToCrouch(false);
	MyFighter->Jump();
}

void UFighterStateComponent::Block()
{
	bIsBlockButtonDown = true;
}

void UFighterStateComponent::StopBlock()
{
	bIsBlockButtonDown = false;
}

void UFighterStateComponent::OnLand()
{
	if (IsMidMove())
	{
		if (CurrentWindow.LandingInterrupts)
		{
			OnMovementTimer();
			CurrentWindowEnd = EWindowEnd::WE_LandInt;
			OnCurrentWindowWinddownFinished();
		}
		else if (CurrentWindow.LandingEndsWindow)
		{
			OnMovementTimer();
			CurrentWindowEnd = EWindowEnd::WE_LandSkip;
			OnCurrentWindowWinddownFinished();
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
	CurrentWindowStage = EWindowStage::WE_Windup;
	PlayerAttackerEffects(CurrentWindow.WindupSFX);
	ActorsToIgnore.Empty();
	ActorsToIgnore.Add(GetOwner());

	if (CurrentWindow.AttackerMovement.IsValid())
	{
		if (MyFighterWorld != nullptr)
		{
			MyFighterWorld->ApplyMovementToActor(GetOwner(), GetOwner(), GetOwnerController(), CurrentWindow.AttackerMovement);
		}
	}
	if (MyFighter != nullptr && !CurrentWindow.AnimName.IsNone())
	{
		if (MyFighter->GetAnimTable() == nullptr)
		{
			UE_LOG(LogBBAnimation, Error, TEXT("%s does not have a valid animation table set."), *GetOwner()->GetName());
		}
		else
		{
			FMoveAnimation *MoveAnimation = MyFighter->GetAnimTable()->FindRow<FMoveAnimation>(CurrentWindow.AnimName, cs, true);
			if (MoveAnimation == nullptr)
			{
				UE_LOG(LogBBAnimation, Error, TEXT("%s move tried to use animation %s, but it was not found in the animation table."), *GetOwner()->GetName(), *CurrentWindow.AnimName.ToString())
			}
			else
			{
				CurrentMontage = MoveAnimation->AntisipationAnimMontage;
				if (CurrentMontage != nullptr)
				{
					ACharacter *Character = Cast<ACharacter>(MyFighter);
					if (Character != nullptr)
					{
						float Duration = Character->PlayAnimMontage(CurrentMontage);
						if (Duration == 0.f)
						{
							UE_LOG(LogBBAnimation, Warning, TEXT("%s::StartCurrentWindowWindup PlayAnimMontage(%s) returned 0 as duration."), *GetNameSafe(this), *GetNameSafe(CurrentMontage));
						}
						else
						{
							UE_LOG(LogBBAnimation, Verbose, TEXT("%s::StartCurrentWindowWindup PlayAnimMontage(%s) duration %f."), *GetNameSafe(this), *GetNameSafe(CurrentMontage), Duration);
						}
					}
					else
					{
						UE_LOG(LogBBAnimation, Warning, TEXT("%s::StartCurrentWindowWindup unable to cast fighter to character."), *GetNameSafe(this));
					}
				}
			}
		}
	}
	if (CurrentWindow.IgnoreCollisions)
	{
		MyFighter->SetFighterCollisions(false);
	}

	if (CurrentWindow.Windup <= 0)
	{
		// No windup, proceed.
		StartCurrentWindowDuration();
	}
	else
	{
		if (
			MyFighterWorld != nullptr
			&& MyFighterWorld->GetMusicBox() != nullptr
			&& Cast<IMusicBox>(MyFighterWorld->GetMusicBox()) != nullptr
			)
		{
			IMusicBox *MusicBox = Cast<IMusicBox>(MyFighterWorld->GetMusicBox());
			float AccInTime = (1.f - CurrentWindowAccuracy) * MusicBox->GetTimeBetweenBeats();
			if ((MyMoveset == nullptr || MyMoveset->GetCurrentWindowInMove() != 0)
				|| MusicBox->GetTimeBetweenBeats() - AccInTime < CurrentWindow.Windup)
			{
				bSkipWindupOnBeat = false;
				GetOwner()->GetWorldTimerManager().SetTimer(
					TimerHandle_Window,
					this,
					&UFighterStateComponent::OnCurrentWindowWindupFinished,
					MyFighterWorld->GetScaledTime(CurrentWindow.Windup),
					false
				);
			}
			else
			{
				bSkipWindupOnBeat = true;
			}
		}
	}
}

void UFighterStateComponent::OnCurrentWindowWindupFinished()
{
	StartCurrentWindowDuration();
}

void UFighterStateComponent::SkipWindupOnBeat()
{
	if (bSkipWindupOnBeat)
	{
		bSkipWindupOnBeat = false;
		SkipWindup();
	}
}

void UFighterStateComponent::SkipWindup()
{
	if (CurrentWindowStage == EWindowStage::WE_Windup)
	{
		if (MyMoveset != nullptr && MyMoveset->GetCurrentWindowInMove() == 0)
		{
			if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Window))
			{
				GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Window);
			}
			OnCurrentWindowWindupFinished();
		}
	}
}

void UFighterStateComponent::StartCurrentWindowDuration()
{

	if (MyFighter != nullptr && !CurrentWindow.AnimName.IsNone())
	{
		if (MyFighter->GetAnimTable() == nullptr)
		{
			UE_LOG(LogBBAnimation, Error, TEXT("%s does not have a valid animation table set."), *GetOwner()->GetName());
		}
		else
		{
			FMoveAnimation *MoveAnimation = MyFighter->GetAnimTable()->FindRow<FMoveAnimation>(CurrentWindow.AnimName, cs, true);
			if (MoveAnimation == nullptr)
			{
				UE_LOG(LogBBAnimation, Error, TEXT("%s move tried to use animation %s, but it was not found in the animation table."), *GetOwner()->GetName(), *CurrentWindow.AnimName.ToString())
			}
			else
			{
				CurrentMontage = MoveAnimation->ExecutionAnimMontage;
				if (CurrentMontage != nullptr)
				{
					ACharacter *Character = Cast<ACharacter>(MyFighter);
					if (Character != nullptr)
					{
						float Duration = Character->PlayAnimMontage(CurrentMontage);
						if (Duration == 0.f)
						{
							UE_LOG(LogBBAnimation, Warning, TEXT("%s::StartCurrentWindowWindup PlayAnimMontage(%s) returned 0 as duration."), *GetNameSafe(this), *GetNameSafe(CurrentMontage));
						}
						else
						{
							UE_LOG(LogBBAnimation, Verbose, TEXT("%s::StartCurrentWindowWindup PlayAnimMontage(%s) duration %f."), *GetNameSafe(this), *GetNameSafe(CurrentMontage), Duration);
						}
					}
					else
					{
						UE_LOG(LogBBAnimation, Warning, TEXT("%s::StartCurrentWindowWindup unable to cast fighter to character."), *GetNameSafe(this));
					}
				}
			}
		}
	}

	CurrentWindowStage = EWindowStage::WE_Duration;
	PlayerAttackerEffects(CurrentWindow.SFX);
	if (CurrentWindow.Duration <= 0)
	{
		// Window has no duration.
		if (CurrentWindow.IsHitboxActive)
		{
			PerformHitboxScan();
		}
		OnCurrentWindowDurationFinished();
	}
	else
	{
		if (CurrentWindow.IsHitboxActive)
		{
			bIsHitboxActive = true;
			SetComponentTickEnabled(true);
		}
		if (CurrentWindow.IsGravityScaled)
		{
			AdjustGravity(CurrentWindow.GravityScale);
		}
		if (MyFighterWorld != nullptr)
		{
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_Window,
				this,
				&UFighterStateComponent::OnCurrentWindowDurationFinished,
				MyFighterWorld->GetScaledTime(CurrentWindow.Duration),
				false
			);
		}
	}
}

void UFighterStateComponent::OnCurrentWindowDurationFinished()
{
	bIsHitboxActive = false;
	if (CurrentWindow.IsGravityScaled)
	{
		AdjustGravity(1.f);
	}

	TryDisableTick();
	StartCurrentWindowWinddown();
}

void UFighterStateComponent::AdjustGravity(float Amount)
{
	UCharacterMovementComponent *MoveComponent = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	if (MoveComponent != nullptr)
	{
		const AActor *CDO = GetDefault<AActor>(GetOwner()->GetClass());
		if (CDO != nullptr)
		{
			UCharacterMovementComponent *CDMoveComponent = CDO->FindComponentByClass<UCharacterMovementComponent>();
			if (CDMoveComponent != nullptr)
			{
				MoveComponent->GravityScale = CDMoveComponent->GravityScale * Amount;
			}
		}
	}
}

void UFighterStateComponent::StartCurrentWindowWinddown()
{
	CurrentWindowStage = EWindowStage::WE_Winddown;
	bHasMoveWindowHit = false;
	if (CurrentWindow.Winddown <= 0)
	{
		// Window has no winddown.
		OnCurrentWindowWinddownFinished();
	}
	else
	{
		if (MyFighterWorld != nullptr)
		{
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_Window,
				this,
				&UFighterStateComponent::OnCurrentWindowWinddownFinished,
				MyFighterWorld->GetScaledTime(CurrentWindow.Winddown),
				false
			);
		}
	}
}

void UFighterStateComponent::OnCurrentWindowWinddownFinished()
{
	if (bHasMoveWindowHit == false && CurrentWindowEnd == EWindowEnd::WE_Missed)
	{
		PlayerAttackerEffects(CurrentWindow.MissSFX);
		if (MyFighter != nullptr)
		{
			MyFighter->MissBeat();
		}
	}
	//Uses CurrentWindowEnd so landing interrutps can still play winddown.
	EndWindow(CurrentWindowEnd);
}

void UFighterStateComponent::EndWindow(EWindowEnd WindowEnd)
{
	if (IsMidMove())
	{
		CurrentWindowStage = EWindowStage::WE_None;
		if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_Window))
		{
			GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Window);
		}
		if (CurrentWindow.IsGravityScaled)
		{
			AdjustGravity(1.f);
		}
		if (MyMoveset != nullptr)
		{
			MyMoveset->OnWindowFinished(WindowEnd);
		}
		if (!MyFighter->IsJumping() && CurrentWindow.IgnoreCollisions)
		{
			MyFighter->SetFighterCollisions(true);
		}
	}
}

void UFighterStateComponent::PerformHitboxScan()
{

	if (MyFighterWorld != nullptr)
	{
		FMoveHitbox WorldHitbox;
		WorldHitbox.Radius = CurrentWindow.Hitbox.Radius;
		WorldHitbox.Origin = CurrentWindow.Hitbox.Origin;
		WorldHitbox.End = CurrentWindow.Hitbox.End;

		//Make hitboxes relative to facing
		if (MyFighter != nullptr)
		{
			WorldHitbox.Origin.X *= MyFighter->GetFacing();
			WorldHitbox.End.X *= MyFighter->GetFacing();
		}

		FHitResult HitResult = MyFighterWorld->TraceHitbox(
			GetOwner()->GetActorLocation(),
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
				//	if (GEngine)
				//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("HIT YOU WITH!!!! %f"), CurrentWindowAccuracy));
					EHitResponse Response = MyFighterWorld->HitActor(
						HitResult.Actor,
						MyFighterWorld->GetDamageType(
							MyFighter->GetStance(),
							CurrentWindow.DamageTypeOverride
						),
						CurrentWindow.DefenderHit,
						CurrentWindow.DefenderBlock,
						CurrentWindowAccuracy,
						GetOwner(),
						(Pawn == nullptr) ? nullptr : Pawn->GetController()
					);

					FTransform ImpactTransform = FTransform({ 0 ,HitResult.ImpactNormal.Rotation().Yaw ,HitResult.ImpactNormal.Rotation().Roll }, HitResult.ImpactPoint, FVector::OneVector);
					FTransform RelativeTransform;
					switch (Response)
					{
					case EHitResponse::HE_Hit:
						CurrentWindowEnd = EWindowEnd::WE_Hit;
						if (MyFighterWorld != nullptr)
						{
							if (MyFighterWorld->IsOnBeat(CurrentWindowAccuracy))
							{
								MyFighter->HitOnBeatLogic();
							}
						}
						bHasMoveWindowHit = true;
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
						if (CurrentWindow.DefenderHit.EndsCurrentWindow)
						{
							EndWindow(EWindowEnd::WE_Hit);
						}
						if (CurrentWindow.BeginsSolo && MyFighterWorld != nullptr)
						{
							MyFighterWorld->StartSolo(TWeakObjectPtr<AActor>(GetOwner()));
						}
						break;
					case EHitResponse::HE_Blocked:
						bHasMoveWindowHit = true;
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
						if (CurrentWindow.DefenderBlock.EndsCurrentWindow)
						{
							EndWindow(EWindowEnd::WE_Missed);
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
	EndDDR();
	if (!bIsKnockedDown)
	{
		if (MyInputParser != nullptr)
		{
			MyInputParser->OnControlReturned();
		}
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
	if ((!IsMidMove() || !CurrentWindow.IsHitboxActive || !bIsHitboxActive) && !bIsBeingMoved)
	{
		SetComponentTickEnabled(false);
	}
}

void UFighterStateComponent::OnMovementTimer()
{
	bIsBeingMoved = false;
	TryDisableTick();
	MyFighterWorld->AdjustLocation(GetOwner());
}

AController* UFighterStateComponent::GetOwnerController() const
{
	APawn *PawnOwner = Cast<APawn>(GetOwner());
	if (PawnOwner != nullptr) return PawnOwner->Controller;
	return nullptr;
}

float UFighterStateComponent::GetSpecial() const
{
	if (MyFighterPlayerState != nullptr)
	{
		return MyFighterPlayerState->GetSpecial();
	}
	return 0.f;
}

void UFighterStateComponent::AddSpecial(float Amount)
{
	if (MyFighterPlayerState != nullptr)
	{
		MyFighterPlayerState->AddSpecial(Amount);
	}
}

bool UFighterStateComponent::UseSpecial(float Amount)
{
	if (MyFighterPlayerState != nullptr)
	{
		return MyFighterPlayerState->UseSpecial(Amount);
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

float UFighterStateComponent::GetCurrentHorizontalMovement() const
{
	if (bIsBeingMoved)
	{
		if (CurrentMovement.UseDeltaAsSpeed)
		{
			return CurrentMovement.Delta.X;
		}
		return CurrentMovement.Delta.X / CurrentMovement.Duration;
	}
	return 0.f;
}

float UFighterStateComponent::GetCurrentVerticalDirection() const
{
	return VerticalDirection;
}

float UFighterStateComponent::GetCurrentHorizontalDirection() const
{
	return HorizontalDirection;
}

void UFighterStateComponent::EndSolo()
{
	if (MyFighterWorld != nullptr)
	{
		MyFighterWorld->EndSolo();
	}
}

void UFighterStateComponent::OnSoloStart(AActor *ActorSoloing)
{
	if (ActorSoloing == nullptr) return;
	if (ActorSoloing == GetOwner())
	{
		bIsFrozenForSolo = false;
		MyMoveset->OnSoloStart();
	}
	else
	{
		bIsFrozenForSolo = true;
	}
}

void UFighterStateComponent::OnSoloEnd()
{
	bIsFrozenForSolo = false;
}

void UFighterStateComponent::Knockdown()
{
	if (!bIsKnockedDown)
	{
		TimesHitThisKnockdown = 0;
		bIsKnockedDown = true;
		StartDDR();
	}
	if (GetOwner() != nullptr
		&& Cast<AFighterCharacter>(GetOwner()) != nullptr
		&& Cast<AFighterCharacter>(GetOwner())->GetCharacterMovement() != nullptr
		&& Cast<AFighterCharacter>(GetOwner())->GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling
		&& Cast<AFighterCharacter>(GetOwner())->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero()
		)
	{
		// If we're not going to land, start the timer immediately.
		UE_LOG(LogKnockdown, Verbose, TEXT("%s immediate knockdown recovery."), *GetNameSafe(GetOwner()));
		KnockdownRecovery(Cast<AFighterCharacter>(GetOwner())->RecoveryDuration);
	}
}

void UFighterStateComponent::KnockdownRecovery(float Duration)
{
	TimesHitThisKnockdown = 0;
	StartInvulnerableTimer(Duration);
}

void UFighterStateComponent::StartInvulnerableTimer(float Duration)
{
	if (Duration <= 0)
	{
		OnInvulnerableTimer();
	}
	else
	{
		GetOwner()->GetWorldTimerManager().SetTimer(
			TimerHandle_Invulnerable,
			this,
			&UFighterStateComponent::OnInvulnerableTimer,
			Duration,
			false
		);
	}
}

void UFighterStateComponent::OnInvulnerableTimer()
{
	bIsKnockedDown = false;
	EndDDR();
	if (!IsStunned())
	{
		if (MyInputParser != nullptr)
		{
			MyInputParser->OnControlReturned();
		}
	}
}

bool UFighterStateComponent::IsIgnoringCollision() const
{
	return IsMidMove() && CurrentWindow.IgnoreCollisions;
}

float UFighterStateComponent::GetCurrentWindowAccuracy() const
{
	if (IsMidMove())
	{
		return CurrentWindowAccuracy;
	}
	return -1;
}

bool UFighterStateComponent::DoesWindowUseHitbox() const
{
	return IsMidMove() && CurrentWindow.IsHitboxActive;
}

FMoveHitbox UFighterStateComponent::GetHitbox() const
{
	if (DoesWindowUseHitbox())
	{
		return CurrentWindow.Hitbox;
	}
	return FMoveHitbox();
}

EWindowStage UFighterStateComponent::GetWindowStage() const
{
	return CurrentWindowStage;
}

int UFighterStateComponent::GetTimesHitThisKnockdown() const
{
	return TimesHitThisKnockdown;
}

void UFighterStateComponent::AddHit()
{
	if (bIsKnockedDown)
	{
		TimesHitThisKnockdown++;
	}
}

bool UFighterStateComponent::IsInDDR()
{
	return bIsInDDR;
}

void UFighterStateComponent::StartDDR()
{
	if (!bIsInDDR && IsStunned() || IsKnockedDown())
	{
		bIsInDDR = true;
		Cast<AFighterCharacter>(MyFighter)->K2_OnToggleDDR(bIsInDDR);
	}
}

void UFighterStateComponent::EndDDR()
{
	if (bIsInDDR && !IsStunned() && !IsKnockedDown())
	{
		bIsInDDR = false;
		Cast<AFighterCharacter>(MyFighter)->K2_OnToggleDDR(bIsInDDR);
	}
}

UBasicFretboard* UFighterStateComponent::GetFretboard()
{
	if (MyMoveset != nullptr)
		return Cast<UMovesetComponent>(MyMoveset)->GetBGFretboard();
	return nullptr;
}

bool UFighterStateComponent::Grabbed(const FVector OpponentLocation)
{
	if (bGrabbed)
		return false;
	if (MyFighter && MyFighter->GetOpponentDirection() > 0)
	{
		Cast<AActor>(MyFighter)->SetActorLocation(FVector(OpponentLocation.X + 50, OpponentLocation.Y, OpponentLocation.Z));
	}
	else
	{
		Cast<AActor>(MyFighter)->SetActorLocation(FVector(OpponentLocation.X - 50, OpponentLocation.Y, OpponentLocation.Z));
	}
	bGrabbed = true;
	return true;
}

bool UFighterStateComponent::Released()
{
	if (!bGrabbed)
		return false;
	bGrabbed = false;
	return true;
}