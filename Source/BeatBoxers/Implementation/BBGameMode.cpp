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
#include "BBWorldSettings.h"

ABBGameMode::ABBGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AFighterCharacter::StaticClass();
	PlayerControllerClass = ABBPlayerController::StaticClass();
	PlayerStateClass = ABBPlayerState::StaticClass();
	GameStateClass = ABBGameState::StaticClass();
	DefaultMusicBoxClass = ABasicMusicBox::StaticClass();

	HitscanDistanceConstant = 100.f;
	bDrawDebugTraces = true;
}

EFighterDamageType ABBGameMode::GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const
{
	// We might want to mess with this in a different gamemode, but here we don't care.
	return DesiredOverride;
}

struct FHitResult ABBGameMode::TraceHitbox(FVector Source, FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors)
{
	FHitResult Result;

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(IgnoreActors);

	FVector Origin = Source + FVector(Hitbox.Origin.X, 0, Hitbox.Origin.Y);
	FVector End = Source + FVector(Hitbox.End.X, 0, Hitbox.End.Y);

	GetWorld()->SweepSingleByObjectType(
		Result,
		Origin,
		End,
		FQuat::Identity,
		FCollisionObjectQueryParams::AllDynamicObjects,
		FCollisionShape::MakeSphere(Hitbox.Radius),
		Params
	);

	if (bDrawDebugTraces)
	{
		DrawDebugLine(
			GetWorld(),
			Origin,
			End,
			FColor::Red,
			false,
			-1.f,
			(uint8)'\000',
			Hitbox.Radius
		);
	}
	return Result;
}

EHitResponse ABBGameMode::HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController)
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
	if (Fighter->IsInvulnerable())
	{
		//TODO: Should this be a miss?
		return EHitResponse::HE_Missed;
	}

	bool WasBlocked = DoesBlock(Fighter, DamageType);

	FImpactData* ImpactData = (WasBlocked) ? &Block : &Hit;
	FImpactData ScaledImpact = GetScaledImpactData(*ImpactData, Accuracy);
	if (ApplyMovementToActor(Actor, Source, SourceController, ScaledImpact.ImpartedMovement) == 1 && !ScaledImpact.ImpartedMovement.UsePhysicsLaunch)
	{
		UE_LOG(LogABBGameMode, Verbose, TEXT("%s::HitActor actor backed into wall, applying to source."), *GetNameSafe(this));
		//The target is already pushed up against a wall, push back the source instead.
		if (Source.IsValid() && SourceController.IsValid() && Source.Get() == SourceController.Get()->GetPawn())
		{
			//Don't do this for projectiles.
			ApplyMovementToActor(Source, Source, SourceController, -ScaledImpact.ImpartedMovement);
		}
	}
	if (ImpactData->bKnocksDown)
	{
		IFighter* Fighter = Cast<IFighter>(Actor.Get());
		if (Fighter != nullptr)
		{
			Fighter->Knockdown();
		}
	}

	if (SourceController.IsValid() && SourceController.Get()->PlayerState != nullptr)
	{
		SourceController.Get()->PlayerState->Score += ScaledImpact.Damage;
		AddSpecial(SourceController.Get()->PlayerState, ScaledImpact.SpecialGenerated);
	}
	return (WasBlocked) ? EHitResponse::HE_Blocked : EHitResponse::HE_Hit;
}

bool ABBGameMode::IsOnBeat(float Accuracy)
{
	float AccInTime = -1;
	IMusicBox* MusicBox = Cast<IMusicBox>(GetMusicBox());
	if (MusicBox != nullptr)
	{
		AccInTime = (1.f - Accuracy) * MusicBox->GetTimeBetweenBeats();
		if (AccInTime <= AccuracyWindowSize/2 || AccInTime >= MusicBox->GetTimeBetweenBeats() - AccuracyWindowSize/2)
		{
			UE_LOG(LogBeatTiming, Verbose, TEXT("IsOnBeat(%f) with AccuracyWindowSize %f? True, AccInTime +%f -%f.")
				, Accuracy
				, AccuracyWindowSize
				, AccInTime
				, MusicBox->GetTimeBetweenBeats() - AccInTime
			);
			return true;
		}
		UE_LOG(LogBeatTiming, Verbose, TEXT("IsOnBeat(%f) with AccuracyWindowSize %f? False, AccInTime +%f -%f.")
			, Accuracy
			, AccuracyWindowSize
			, AccInTime
			, MusicBox->GetTimeBetweenBeats() - AccInTime
		);
		return false;
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("ABBGameMode::IsOnBeat MusicBox nullptr."));
	}
	return false;
}

void ABBGameMode::AddSpecial(APlayerState *PlayerState, float Amount)
{
	ABBPlayerState *BBPlayerState = Cast<ABBPlayerState>(PlayerState);
	if (BBPlayerState != nullptr)
	{
		BBPlayerState->AddSpecial(Amount);
	}
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
		FVector InitialCameraLocation = FVector(0, 800, 180);
		FVector InitialCameraLookAtLocation = FVector(0, 0, 180);
		ABBWorldSettings *Settings = Cast<ABBWorldSettings>(GetWorldSettings());
		if (Settings != nullptr)
		{
			InitialCameraLocation = Settings->InitialCameraLocation;
			InitialCameraLookAtLocation = Settings->InitialCameraLookAtLocation;
		}
		ACameraActor* Camera = GetWorld()->SpawnActor<ACameraActor>(
			InitialCameraLocation
			, FRotationMatrix::MakeFromX(InitialCameraLookAtLocation - InitialCameraLocation).Rotator()
			, FActorSpawnParameters()
			);
		GetGameState<ABBGameState>()->MainCamera = Cast<ACameraActor>(Camera);
		for (TActorIterator<APlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			APlayerController *PC = *ActorItr;
			if (PC != nullptr)
			{
				PC->SetViewTarget(Camera);
			}
		}
		
		IMusicBox *WorldMusicBox = GetGameState<ABBGameState>()->GetIMusicBox();
		if (WorldMusicBox != nullptr)
		{
			WorldMusicBox->StartMusic();
		}
		else
		{
			UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("Gamemode unable to get musicbox from gamestate."));
		}
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("Gamemode unable to get gamestate as ABBGameState."));
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
		//Invalid or no movement.
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
		IFighter *Fighter = Cast<IFighter>(Source.Get());
		if (Fighter != nullptr)
		{
			NonrelativeMovement.Delta.X *= Fighter->GetFacing();
			NonrelativeMovement.IsRelativeToAttackerFacing = false;
		}
	}

	FHitResult Result;
	GetWorld()->LineTraceSingleByObjectType(
		Result,
		Target.Get()->GetActorLocation(),
		Target.Get()->GetActorLocation() + FVector{1.f, 0.f, 0.f}.GetSafeNormal() * FMath::Sign(NonrelativeMovement.Delta.X) * HitscanDistanceConstant,
		FCollisionObjectQueryParams::AllStaticObjects,
		FCollisionQueryParams::DefaultQueryParam
	);

	if (!Movement.UsePhysicsLaunch)
	{
		IFighter *TargetFighter = Cast<IFighter>(Target.Get());
		if (TargetFighter != nullptr)
		{
			TargetFighter->ApplyMovement(NonrelativeMovement);
		}
	}
	else
	{
		ACharacter* Character = Cast<ACharacter>(Target.Get());
		if (Character != nullptr)
		{
			FVector Launch{NonrelativeMovement.Delta.X, 0.f, NonrelativeMovement.Delta.Y};
			Character->LaunchCharacter(Launch, true, true);
			if (Character->GetCapsuleComponent() != nullptr)
			{
				Character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			}
		}
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

void ABBGameMode::EndSolo()
{
	if (SoloEndEvent.IsBound())
	{
		SoloEndEvent.Broadcast();
	}
}

void ABBGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameMode::InitGame(MapName, Options, ErrorMessage);
}

void ABBGameMode::InitGameState()
{
	Super::InitGameState();
	if (DefaultMusicBoxClass.Get() != nullptr)
	{
		UObject *Object = GetWorld()->SpawnActor(DefaultMusicBoxClass);
		IMusicBox *MusicBox = Cast<IMusicBox>(Object);
		if (MusicBox != nullptr)
		{
			GetGameState<ABBGameState>()->SetMusicBox(Object);
			MusicBox->GetMusicEndEvent().AddDynamic(this, &ABBGameMode::OnMusicEnd);
		}
		else
		{
			UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s DefaultMusicBoxClass does not implement IMusicBox."), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s DefaultMusicBoxClass is not set to a valid class."), *GetNameSafe(this));
	}
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

void ABBGameMode::OnMusicEnd()
{
	UE_LOG(LogABBGameMode, Log, TEXT("Music ended, ending game."));
	EndMatch();
}

void ABBGameMode::HandleMatchIsWaitingToStart()
{
	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		FNewGameData NewGameData = GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData;
		if (NumPlayers + NumBots < 2)
		{
			//Create second local player.
			UE_LOG(LogABBGameMode, Log, TEXT("NumPlayers %d, NumBots %d. Adding second local player."), NumPlayers, NumBots);
			FString Err;
			if (GetWorld()->GetGameInstance<UBBGameInstance>()->CreateLocalPlayer(-1, Err, true) == nullptr)
			{
				UE_LOG(LogABBGameMode, Error, TEXT("Unable to add local player: %s"), *Err);
			}
		}
	}
	else
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("GameMode unable to get gameinstance as UBBGameInstance."));
	}



	Super::HandleMatchIsWaitingToStart();
}

FImpactData ABBGameMode::GetScaledImpactData_Implementation(const FImpactData& ImpactData, float Accuracy)
{
	FImpactData OutImpact = ImpactData;
	OutImpact.Damage *= Accuracy;
	OutImpact.SpecialGenerated *= Accuracy;
	return OutImpact;
}

UObject* ABBGameMode::GetMusicBox()
{
		return GetGameState<ABBGameState>()->GetUMusicBox();
}

void ABBGameMode::PlayerHitOnBeat(APlayerController* PlayerController)
{
	if (PlayerController->PlayerState != nullptr)
	{
		ABBPlayerState* BBPlayerState = Cast<ABBPlayerState>(PlayerController->PlayerState);
		if (BBPlayerState != nullptr)
		{
			int OldCombo = BBPlayerState->GetBeatCombo();
			BBPlayerState->SetBeatCombo(OldCombo + 1);
			if (PlayerBeatComboChangedEvent.IsBound())
			{
				PlayerBeatComboChangedEvent.Broadcast(PlayerController, OldCombo + 1);
			}
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Warning, TEXT("ABBGameMode::PlayerHitOnBeat was given a nullptr for a player controller."));
	}
}

void ABBGameMode::PlayerMissBeat(APlayerController* PlayerController)
{
	if (PlayerController->PlayerState != nullptr)
	{
		ABBPlayerState* BBPlayerState = Cast<ABBPlayerState>(PlayerController->PlayerState);
		if (BBPlayerState != nullptr)
		{
			int OldCombo = BBPlayerState->GetBeatCombo();
			BBPlayerState->SetBeatCombo(0);
			if (PlayerBeatComboChangedEvent.IsBound())
			{
				PlayerBeatComboChangedEvent.Broadcast(PlayerController, 0);
			}
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Warning, TEXT("ABBGameMode::PlayerMissBeat was given a nullptr for a player controller."));
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

FPlayerBeatComboChangedEvent& ABBGameMode::GetOnPlayerBeatComboChangedEvent()
{
	return PlayerBeatComboChangedEvent;
}

void ABBGameMode::AdjustCamera()
{
	ABBWorldSettings *Settings = Cast<ABBWorldSettings>(GetWorldSettings());
	if (Settings != nullptr)
	{
		FVector Target;
		FVector2D DesiredExtents;
		if (GameState->PlayerArray.Num() > 1)
		{
			ABBPlayerState *BBPlayerState;
			BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[0]);
			FVector p1loc, p2loc;
			if (BBPlayerState != nullptr && BBPlayerState->MyPawn != nullptr)
			{
				p1loc = BBPlayerState->MyPawn->GetActorLocation();
			}
			else
			{
				p1loc = Settings->InitialCameraLookAtLocation;
			}
			BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[1]);
			if (BBPlayerState != nullptr && BBPlayerState->MyPawn != nullptr)
			{
				p2loc = BBPlayerState->MyPawn->GetActorLocation();
			}
			else
			{
				p2loc = Settings->InitialCameraLookAtLocation;
			}
			FVector Diff = p1loc - p2loc;
			Target = p2loc + (Diff / 2.f);
			DesiredExtents = FVector2D{ FMath::Abs(Diff.X), FMath::Abs(Diff.Z) };
		}
		else if (GameState->PlayerArray.Num() == 1)
		{
			ABBPlayerState *BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[0]);
			if (BBPlayerState != nullptr && BBPlayerState->MyPawn != nullptr)
			{
				Target = BBPlayerState->MyPawn->GetActorLocation();
				DesiredExtents = FVector2D{ 0.f, 0.f };
			}
		}

		DesiredExtents += Settings->CameraPadding;

		FVector CamPos;
		CamPos.X = FMath::Clamp(Target.X, Settings->CameraBounds.Min.X, Settings->CameraBounds.Max.X);
		CamPos.Z = FMath::Clamp(Target.Z, Settings->CameraBounds.Min.Z, Settings->CameraBounds.Max.Z);

		float FOV = GetDefault<APlayerCameraManager>()->DefaultFOV;
		APlayerController *Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (Controller != nullptr && Controller->PlayerCameraManager != nullptr)
		{
			FOV = Controller->PlayerCameraManager->GetFOVAngle();
		}

		CamPos.Y = FMath::Clamp(
			FMath::Max(DesiredExtents.X, DesiredExtents.Y) / 2.f / FMath::Tan(FMath::DegreesToRadians(FOV / 2.f))
			, Settings->CameraBounds.Min.Y
			, Settings->CameraBounds.Max.Y
			);

		if (GetGameState<ABBGameState>() != nullptr)
		{
			ACameraActor *Cam = GetGameState<ABBGameState>()->MainCamera;
			if (Cam != nullptr)
			{
				Cam->SetActorLocation(CamPos);
			}
		}
	}
}

void ABBGameMode::Tick(float DeltaSeconds)
{
	AGameMode::Tick(DeltaSeconds);

	AdjustCamera();
}
