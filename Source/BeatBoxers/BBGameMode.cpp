// copyright 2017 BYU Animation

#include "BBGameMode.h"
#include "IFighter.h"
#include "FighterCharacter.h"
#include "BBPlayerController.h"
#include "BBGameState.h"
#include "BBGameInstance.h"
#include "BasicMusicBox.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ABBGameMode::ABBGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AFighterCharacter::StaticClass();
	PlayerControllerClass = ABBPlayerController::StaticClass();
	GameStateClass = ABBGameState::StaticClass();
	DefaultMusicBoxClass = ABasicMusicBox::StaticClass();

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

	GetWorld()->SweepSingleByObjectType(
		Result,
		Hitbox.Origin,
		Hitbox.End,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllDynamicObjects,
		FCollisionShape::MakeSphere(Hitbox.Radius),
		Params
	);

	DrawDebugLine(
		GetWorld(),
		Hitbox.Origin,
		Hitbox.End,
		FColor::Red,
		false,
		-1.f,
		(uint8)'\000',
		Hitbox.Radius
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

	if (GetBBGameState() != nullptr)
	{
		ACameraActor* Camera = GetWorld()->SpawnActor<ACameraActor>(InitialCameraLocation, FRotationMatrix::MakeFromX(InitialCameraLookAtLocation - InitialCameraLocation).Rotator(), FActorSpawnParameters());
		GetBBGameState()->MainCamera = Cast<ACameraActor>(Camera);
		for (TActorIterator<APlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			APlayerController *PC = *ActorItr;
			if (PC != nullptr)
			{
				PC->SetViewTarget(Camera);
			}
		}

		if (DefaultMusicBoxClass.Get() != nullptr)
		{
			AActor *Actor = GetWorld()->SpawnActorDeferred<AActor>(DefaultMusicBoxClass.Get(), FTransform::Identity);
			if (Actor != nullptr)
			{
				IMusicBox *WorldMusicBox = Cast<IMusicBox>(Actor);
				if (WorldMusicBox != nullptr)
				{
					GetBBGameState()->WorldMusicBox = WorldMusicBox;
				}
				else
				{
					UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s DefaultMusicBoxClass %s does not implement IMusicBox."), *GetNameSafe(this), *GetNameSafe(Actor));
				}
			}
			else
			{
				UE_LOG(LogABBGameMode, Error, TEXT("%s was unable to spawn MusicBox actor."), *GetNameSafe(this));
			}
		}
		else
		{
			UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s DefaultMusicBoxClass is not set to a valid class."), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("%s unable to get gamestate as ABBGameState."), *GetNameSafe(this));
	}

	if (GetBBGameInstance() != nullptr)
	{
		FNewGameData NewGameData = GetBBGameInstance()->NewGameData;
		if (!NewGameData.IsSecondPlayerHuman)
		{
			UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s GameInstance requests second player be AI, this is unimplemented."), *GetNameSafe(this));
		}
		else
		{
			//Create second local player.
			UGameplayStatics::CreatePlayer(GetWorld(), -1, true);
		}
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("GameMode unable to get gameinstance as UBBGameInstance."));
	}
}

ABBGameState* ABBGameMode::GetBBGameState()
{
	return Cast<ABBGameState>(GameState);
}

UBBGameInstance* ABBGameMode::GetBBGameInstance()
{
	return Cast<UBBGameInstance>(GetWorld()->GetGameInstance());
}

void ABBGameMode::ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement& Movement)
{
	if (!Movement.IsValid())
	{
		UE_LOG(LogABBGameMode, Warning, TEXT("ABBGameMode asked to apply invalid movement to actor."));
		return;
	}

	if (!Target.IsValid())
	{
		UE_LOG(LogABBGameMode, Warning, TEXT("ABBGameMode asked to apply movement to invalid actor."));
		return;
	}

	UE_LOG(LogABBGameMode, Verbose, TEXT("ABBGameMode asked to apply Movement(%s) to actor %s."), *Movement.ToString(), *GetNameSafe(Target.Get()));

	FMovement NonrelativeMovement = Movement;
	if (Movement.IsRelativeToAttackerFacing && Source.IsValid())
	{
		FTransform Transform = Source.Get()->GetActorTransform();
		FRotator Rotator = Source.Get()->GetActorRotation();
		NonrelativeMovement.Delta = Rotator.RotateVector(NonrelativeMovement.Delta);
		NonrelativeMovement.IsRelativeToAttackerFacing = false;
	}

	IFighter *TargetFighter = Cast<IFighter>(Target.Get());
	if (TargetFighter != nullptr)
	{
		TargetFighter->ApplyMovement(NonrelativeMovement);
	}
}

void ABBGameMode::StartSolo(TWeakObjectPtr<AActor> OneSoloing)
{
	if (OneSoloing.IsValid() && SoloStartEvent.IsBound())
	{
		SoloStartEvent.Broadcast(OneSoloing.Get());
	}
}

FSoloStartEvent& ABBGameMode::GetOnSoloStartEvent()
{
	return SoloStartEvent;
}

FSoloEndEvent& ABBGameMode::GetOnSoloEndEvent()
{
	return SoloEndEvent;
}

void ABBGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameMode::InitGame(MapName, Options, ErrorMessage);
}

UClass* ABBGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController == nullptr)
	{
		UE_LOG(LogABBGameMode, Error, TEXT("%s::GetDefaultPawnClassForController(nullptr), should not be nullptr."), *GetNameSafe(this));
		return DefaultPawnClass;
	}
	UE_LOG(LogABBGameMode, Verbose, TEXT("%s::GetDefaultPawnClassForController(%s) called."), *GetNameSafe(this), *GetNameSafe(InController));
	APlayerController *PlayerController = Cast<APlayerController>(InController);
	if (PlayerController != nullptr)
	{
		if (GetBBGameInstance() != nullptr)
		{
			FNewGameData NewGameData = GetBBGameInstance()->NewGameData;
			switch (UGameplayStatics::GetPlayerControllerID(PlayerController))
			{
			case 0:
				return (NewGameData.Player0Class != nullptr) ? NewGameData.Player0Class : DefaultPawnClass;
				break;
			case 1:
				return (NewGameData.Player1Class != nullptr) ? NewGameData.Player1Class : DefaultPawnClass;
				break;
			default:
				UE_LOG(LogABBGameMode, Error, TEXT("%s::GetDefaultPawnClassForController(%s), controller ID is neither 0 nor 1, unimplemented."), *GetNameSafe(this), *GetNameSafe(InController));
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogABBGameMode, Error, TEXT("%s::GetDefaultPawnClassForController(%s), controller is not a player controller, unimplemented."), *GetNameSafe(this), *GetNameSafe(InController));
	}
	return DefaultPawnClass;
}
