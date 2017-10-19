// copyright 2017 BYU Animation

#include "BBGameMode.h"
#include "Interfaces/IFighter.h"
#include "Character/FighterCharacter.h"
#include "BBPlayerController.h"
#include "BBGameState.h"
#include "BBGameInstance.h"
#include "BBPlayerStart.h"
#include "BBPlayerState.h"
#include "BasicMusicBox.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ABBGameMode::ABBGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AFighterCharacter::StaticClass();
	PlayerControllerClass = ABBPlayerController::StaticClass();
	PlayerStateClass = ABBPlayerState::StaticClass();
	GameStateClass = ABBGameState::StaticClass();
	DefaultMusicBoxClass = ABasicMusicBox::StaticClass();

	InitialCameraLocation = FVector(0, 800, 180);
	InitialCameraLookAtLocation = FVector(0, 0, 180);
	bDrawDebugTraces = true;
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

	if (bDrawDebugTraces)
	{
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
	}
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
		UE_LOG(LogBeatBoxers, Error, TEXT("GameMode asked to hit a non-fighter actor %s by source %s cont%s"),
			*GetNameSafe(Actor.Get()),
			(Source.IsValid()) ? *GetNameSafe(Source.Get()) : TEXT("INVALID SOURCE"),
			(SourceController.IsValid()) ? *GetNameSafe(SourceController.Get()) : TEXT("INVALID CONTROLLER")
		);
		return EHitResponse::HE_Missed;
	}

	bool WasBlocked = DoesBlock(Fighter, DamageType);

	FImpactData* ImpactData = (WasBlocked) ? &Block : &Hit;
	if (ApplyMovementToActor(Actor, Source, SourceController, ImpactData->ImpartedMovement) == 1)
	{
		UE_LOG(LogABBGameMode, Verbose, TEXT("%s::HitActor actor backed into wall, applying to source."), *GetNameSafe(this));
		//The target is already pushed up against a wall, push back the source instead.
		if (Source.IsValid() && SourceController.IsValid() && Source.Get() == SourceController.Get()->GetPawn())
		{
			//Don't do this for projectiles.
			ApplyMovementToActor(Source, Source, SourceController, -ImpactData->ImpartedMovement);
		}
	}

	if (SourceController.IsValid() && SourceController.Get()->PlayerState != nullptr)
	{
		SourceController.Get()->PlayerState->Score += ImpactData->Damage;
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

	UE_LOG(LogABBGameMode, Log, TEXT("Starting new match."));

	UE_LOG(LogABBGameMode, Verbose, TEXT("\t%d players."), GameState->PlayerArray.Num());
	for (int32 PlayerIndex = 0; PlayerIndex < GameState->PlayerArray.Num(); PlayerIndex++)
	{
		if (GameState->PlayerArray[PlayerIndex] != nullptr)
		{
			UE_LOG(LogABBGameMode, Verbose, TEXT("\tPlayer[%d]->PlayerId = %d"), PlayerIndex, GameState->PlayerArray[PlayerIndex]->PlayerId);
		}
		else
		{
			UE_LOG(LogABBGameMode, Verbose, TEXT("\tPlayer[%d] = nullptr"), PlayerIndex);
		}
	}

	if (GetGameState<ABBGameState>() != nullptr)
	{
		ACameraActor* Camera = GetWorld()->SpawnActor<ACameraActor>(InitialCameraLocation, FRotationMatrix::MakeFromX(InitialCameraLookAtLocation - InitialCameraLocation).Rotator(), FActorSpawnParameters());
		GetGameState<ABBGameState>()->MainCamera = Cast<ACameraActor>(Camera);
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
			IMusicBox *WorldMusicBox = GetGameState<ABBGameState>()->WorldMusicBox;
			if (WorldMusicBox == nullptr)
			{
				AActor *Actor = GetWorld()->SpawnActorDeferred<AActor>(DefaultMusicBoxClass.Get(), FTransform::Identity);
				if (Actor != nullptr)
				{
					WorldMusicBox = Cast<IMusicBox>(Actor);
					if (WorldMusicBox != nullptr)
					{
						GetGameState<ABBGameState>()->WorldMusicBox = WorldMusicBox;
						WorldMusicBox->GetMusicEndEvent().AddDynamic(this, &ABBGameMode::OnMusicEnd);
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
			if (WorldMusicBox != nullptr)
			{
					WorldMusicBox->StartMusic();
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

	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		FNewGameData NewGameData = GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData;
		if (NumPlayers + NumBots < 2)
		{
			//Create second local player.
			UGameplayStatics::CreatePlayer(GetWorld(), -1, true);
		}
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("GameMode unable to get gameinstance as UBBGameInstance."));
	}

	if (GameState != nullptr)
	{
		UE_LOG(LogABBGameMode, Verbose, TEXT("Begin attaching opponents."));
		AActor *P1Char = nullptr, *P2Char = nullptr;
		ABBPlayerState *BBPlayerState = nullptr;
		if (GameState->PlayerArray.Num() >= 2)
		{
			if (GameState->PlayerArray[0] != nullptr)
			{
				BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[0]);
				if (BBPlayerState != nullptr)
				{
					P1Char = BBPlayerState->MyPawn;
				}
				else
				{
					UE_LOG(LogABBGameMode, Verbose, TEXT("Unable to cast playerstate0 to BBPlayerstate"));
				}
			}
			if (GameState->PlayerArray[1] != nullptr)
			{
				BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[1]);
				if (BBPlayerState != nullptr)
				{
					P2Char = BBPlayerState->MyPawn;
				}
				else
				{
					UE_LOG(LogABBGameMode, Verbose, TEXT("Unable to cast playerstate1 to BBPlayerstate"));
				}
			}

			if (P1Char != nullptr && P2Char != nullptr)
			{
				AFighterCharacter *Fighter = Cast<AFighterCharacter>(P1Char);
				if (Fighter != nullptr)
				{
					Fighter->SetOpponent(P2Char);
				}
				else
				{
					UE_LOG(LogABBGameMode, Verbose, TEXT("Unable to cast player0 to Fighter"));
				}
				Fighter = Cast<AFighterCharacter>(P2Char);
				if (Fighter != nullptr)
				{
					Fighter->SetOpponent(P1Char);
				}
				else
				{
					UE_LOG(LogABBGameMode, Verbose, TEXT("Unable to cast player1 to Fighter"));
				}
			}
		}
		else
		{
			UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("GameMode can't find at least two players."));
		}
		UE_LOG(LogABBGameMode, Verbose, TEXT("End attaching opponents."));
	}
}

int ABBGameMode::ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement)
{
	if (!Movement.IsValid())
	{
		UE_LOG(LogABBGameMode, Warning, TEXT("ABBGameMode asked to apply invalid movement to actor."));
		return -1;
	}

	if (!Target.IsValid())
	{
		UE_LOG(LogABBGameMode, Warning, TEXT("ABBGameMode asked to apply movement to invalid actor."));
		return -1;
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

	FHitResult Result;
	GetWorld()->LineTraceSingleByObjectType(
		Result,
		Target.Get()->GetActorLocation(),
		Target.Get()->GetActorLocation() + NonrelativeMovement.Delta.GetSafeNormal() * 100.f,
		FCollisionObjectQueryParams::AllStaticObjects,
		FCollisionQueryParams::DefaultQueryParam
	);

	IFighter *TargetFighter = Cast<IFighter>(Target.Get());
	if (TargetFighter != nullptr)
	{
		TargetFighter->ApplyMovement(NonrelativeMovement);
	}

	return Result.bBlockingHit;
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
	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		FNewGameData NewGameData = GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData;
		int32 PlayerID = InController->PlayerState->PlayerId;
		for (int32 PlayerIndex = 0; PlayerIndex < GameState->PlayerArray.Num(); PlayerIndex++)
		{
			if (PlayerID == GameState->PlayerArray[PlayerIndex]->PlayerId)
			{
				switch (PlayerIndex)
				{
				case 0:
					return (NewGameData.Player0Class != nullptr) ? NewGameData.Player0Class : DefaultPawnClass;
					break;
				case 1:
					return (NewGameData.Player1Class != nullptr) ? NewGameData.Player1Class : DefaultPawnClass;
					break;
				default:
					UE_LOG(LogABBGameMode, Error, TEXT("%s::GetDefaultPawnClassForController(%s), PlayerIndex %d unexpected value, unimplemented."), *GetNameSafe(this), *GetNameSafe(InController), PlayerIndex);
					break;
				}
			}
		}
	}
	return DefaultPawnClass;
}

AActor* ABBGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	int32 PlayerID = Player->PlayerState->PlayerId;
	int32 PlayerIndex;
	AActor *Backup = nullptr;
	for (TActorIterator<ABBPlayerStart> It(GetWorld()); It; ++It)
	{
		ABBPlayerStart* PlayerStart = *It;
		PlayerIndex = PlayerStart->PlayerIndex;
		if (PlayerIndex < 0)
		{
			if (Backup == nullptr)
			{
				Backup = PlayerStart;
			}
		}
		else if (GameState->PlayerArray.Num() > PlayerIndex)
		{
			if (GameState->PlayerArray[PlayerIndex] != nullptr)
			{
				if (PlayerID == GameState->PlayerArray[PlayerIndex]->PlayerId)
				{
					return PlayerStart;
				}
			}
		}
	}
	UE_LOG(LogABBGameMode, Warning, TEXT("%s::ChoosePlayerStart(%s) was unable to find a PlayerStart specifically designated for player %d."), *GetNameSafe(this), *GetNameSafe(Player), PlayerID);
	if (Backup != nullptr)
	{
		return Backup;
	}
	return AGameMode::ChoosePlayerStart_Implementation(Player);
}

APawn* ABBGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn *Pawn = AGameModeBase::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

	ABBPlayerState *BBPlayerState = Cast<ABBPlayerState>(NewPlayer->PlayerState);
	if (BBPlayerState != nullptr)
	{
		BBPlayerState->MyPawn = Pawn;
	}

	return Pawn;
}

void ABBGameMode::AdjustLocation(AActor *ActorToAdjust)
{
	BPAdjustLocation(ActorToAdjust);
}

void ABBGameMode::BPAdjustLocation_Implementation(AActor *ActorToAdjust)
{
	//nop
}

void ABBGameMode::OnMusicEnd()
{
	UE_LOG(LogABBGameMode, Log, TEXT("Music ended, ending game."));
	EndMatch();
}