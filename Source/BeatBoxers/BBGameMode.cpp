// copyright 2017 BYU Animation

#include "BBGameMode.h"
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
	//TODO
	return DesiredOverride;
}

struct FHitResult ABBGameMode::TraceHitbox(FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors)
{
	//TODO
	return FHitResult();
}

EHitResponse ABBGameMode::HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController)
{
	//TODO
	return EHitResponse::HE_Hit;
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