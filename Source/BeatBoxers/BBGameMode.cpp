// copyright 2017 BYU Animation

#include "BBGameMode.h"
#include "Fighter.h"
#include "FighterCharacter.h"
#include "BBPlayerController.h"
#include "BBGameState.h"
#include "EngineUtils.h"

ABBGameMode::ABBGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AFighterCharacter::StaticClass();
	PlayerControllerClass = ABBPlayerController::StaticClass();
	GameStateClass = ABBGameState::StaticClass();

	InitialCameraLocation = FVector(0, 500, 180);
	InitialCameraLookAtLocation = FVector(0, 0, 180);
}

EFighterDamageType ABBGameMode::GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const
{
	// We might want to mess with this in a different gamemode, but here we don't care.
	return DesiredOverride;
}

struct FHitResult ABBGameMode::TraceHitbox(FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors)
{
	FHitResult Result;

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(IgnoreActors);
	Params.TraceTag = GetWorld()->DebugDrawTraceTag;

	GetWorld()->SweepSingleByObjectType(
		Result,
		Hitbox.Origin,
		Hitbox.End,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllDynamicObjects,
		FCollisionShape::MakeSphere(Hitbox.Radius),
		Params
	);

	return Result;
}

EHitResponse ABBGameMode::HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController)
{
	if (!Actor.IsValid())
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("GameMode asked to hit an invalid actor by source %s contoller %s."),
			(Source.IsValid()) ? *GetNameSafe(Source.Get()) : TEXT("INVALID SOURCE"),
			(SourceController.IsValid()) ? *GetNameSafe(SourceController.Get()) : TEXT("INVALID CONTROLLER")
		);
		return EHitResponse::HE_Missed;
	}

	IFighter *Fighter = Cast<IFighter>(Actor.Get());
	if (Fighter == nullptr)
	{
		//Behavior not defined for non-fighters.
		UE_LOG(LogBeatBoxers, Error, TEXT("GameMode asked to hit a non-fighter actor %s by source %s controller %s"),
			*GetNameSafe(Actor.Get()),
			(Source.IsValid()) ? *GetNameSafe(Source.Get()) : TEXT("INVALID SOURCE"),
			(SourceController.IsValid()) ? *GetNameSafe(SourceController.Get()) : TEXT("INVALID CONTROLLER")
		);
		return EHitResponse::HE_Missed;
	}

	bool WasBlocked = DoesBlock(Fighter, DamageType);

	FImpactData* ImpactData = (WasBlocked) ? &Block : &Hit;
	ApplyMovementToActor(Actor, Source, SourceController, ImpactData->ImpartedMovement);

	if (GetBBGameState() != nullptr && SourceController.IsValid())
	{
		GetBBGameState()->AddScore(SourceController.Get(), ImpactData->Damage);
	}

	return (WasBlocked) ? EHitResponse::HE_Blocked : EHitResponse::HE_Hit;
}

bool ABBGameMode::DoesBlock(IFighter *Fighter, EFighterDamageType DamageType) const
{
	if (Fighter == nullptr || !Fighter->IsBlocking()) return false;

	switch (Fighter->GetStance())
	{
	case EStance::SE_NA:
		return false;
	case EStance::SE_Standing:
		switch (DamageType)
		{
		case EFighterDamageType::DE_High:
			return true;
		case EFighterDamageType::DE_Low:
			return false;
		case EFighterDamageType::DE_Overhead:
			return true;
		default:
			break;
		}
		break;
	case EStance::SE_Crouching:
		switch (DamageType)
		{
		case EFighterDamageType::DE_High:
			return true;
		case EFighterDamageType::DE_Low:
			return true;
		case EFighterDamageType::DE_Overhead:
			return false;
		default:
			break;
		}
		break;
	case EStance::SE_Jumping:
		return false;
	default:
		break;
	}

	if (DamageType == EFighterDamageType::DE_None) return true;
	return false;
}

void ABBGameMode::StartMatch()
{
	Super::StartMatch();

	UE_LOG(LogBeatBoxers, Log, TEXT("Starting new match."));
	ACameraActor* Camera = GetWorld()->SpawnActor<ACameraActor>(InitialCameraLocation, FRotationMatrix::MakeFromX(InitialCameraLookAtLocation - InitialCameraLocation).Rotator(), FActorSpawnParameters());
	
	if (GetBBGameState() != nullptr)
	{
		GetBBGameState()->MainCamera = Cast<ACameraActor>(Camera);
	}

	for (TActorIterator<APlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APlayerController *PC = *ActorItr;
		if (PC != nullptr)
		{
			PC->SetViewTarget(Camera);
		}
	}
}

ABBGameState* ABBGameMode::GetBBGameState()
{
	return Cast<ABBGameState>(GameState);
}

void ABBGameMode::ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement& Movement)
{
	if (!Movement.IsValid())
	{
		UE_LOG(LogBeatBoxers, Warning, TEXT("ABBGameMode asked to apply invalid movement to actor."));
		return;
	}

	if (!Target.IsValid())
	{
		UE_LOG(LogBeatBoxers, Warning, TEXT("ABBGameMode asked to apply movement to invalid actor."));
		return;
	}

	FMovement NonrelativeMovement = Movement;
	if (Movement.IsRelativeToAttackerFacing && Source.IsValid())
	{
		FTransform Transform = Source.Get()->GetActorTransform();
		FRotator Rotator = Source.Get()->GetActorRotation();
		NonrelativeMovement.Delta = Rotator.RotateVector(NonrelativeMovement.Delta) / NonrelativeMovement.Duration;
	}

	IFighter *TargetFighter = Cast<IFighter>(Target.Get());
	if (TargetFighter != nullptr)
	{
		TargetFighter->ApplyMovement(NonrelativeMovement);
	}
}