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

	DelayBetweenRounds = 2.f;
	RoundsToWin = 2;
	HitscanDistanceConstant = 100.f;
	bDrawDebugTraces = true;
	bIsInRound = false;
	RoundNumber = 0;
	bReadyToEnd = false;
	bDebugMode = false;
	RoundTime = 90;
	DelayBeforeEnd = 2.f;
	DefaultClashImpact.bKnocksDown = false;
	DefaultClashImpact.Damage = 0.f;
	DefaultClashImpact.ImpartedMovement.Delta = FVector2D(-100.f, 0.f);
	DefaultClashImpact.ImpartedMovement.Duration = 0.2f;
	DefaultClashImpact.ImpartedMovement.UseDeltaAsSpeed = false;
	DefaultClashImpact.ImpartedMovement.IsRelativeToAttackerFacing = true;
	DefaultClashImpact.ImpartedMovement.UsePhysicsLaunch = false;
	DefaultClashImpact.StunLength = 0.5f;
	ComboImpactScaling = 0.8f;
	GlobalImpactScaling = 1.f;
}

EFighterDamageType ABBGameMode::GetDamageType(EStance Stance, EFighterDamageType DesiredOverride) const
{
	// We might want to mess with this in a different gamemode, but here we don't care.
	if (DesiredOverride == EFighterDamageType::DE_None)
	{
		switch (Stance)
		{
		case EStance::SE_Crouching:
			return EFighterDamageType::DE_Low;
		case EStance::SE_Standing:
			return EFighterDamageType::DE_High;
		case EStance::SE_Jumping:
			return EFighterDamageType::DE_Overhead;
		}
	}
	return DesiredOverride;
}

struct FHitResult ABBGameMode::TraceHitbox(FVector Source, FMoveHitbox Hitbox, TArray< TWeakObjectPtr<AActor> >& IgnoreActors)
{
	FHitResult Result;

	FCollisionQueryParams QParams;
	QParams.AddIgnoredActors(IgnoreActors);

	FCollisionResponseParams RParams;
	RParams.CollisionResponse.Camera = ECR_Ignore;
	RParams.CollisionResponse.Destructible = ECR_Ignore;
	RParams.CollisionResponse.Pawn= ECR_Block;
	RParams.CollisionResponse.PhysicsBody = ECR_Ignore;
	RParams.CollisionResponse.Vehicle = ECR_Ignore;
	RParams.CollisionResponse.Visibility = ECR_Ignore;
	RParams.CollisionResponse.WorldDynamic = ECR_Ignore;
	RParams.CollisionResponse.WorldStatic = ECR_Ignore;

	FVector Origin = Source + FVector(Hitbox.Origin.X, 0, Hitbox.Origin.Y);
	FVector End = Source + FVector(Hitbox.End.X, 0, Hitbox.End.Y);

	GetWorld()->SweepSingleByChannel(
		Result
		, Origin
		, End
		, FQuat::Identity
		, ECollisionChannel::ECC_GameTraceChannel1
		, FCollisionShape::MakeSphere(Hitbox.Radius)
		, QParams
		, RParams
	);

	if (bDrawDebugTraces)
	{
		FVector dir = (End - Origin).GetSafeNormal();
		FVector op = FVector::CrossProduct(FVector{ 0, 1, 0 }, dir);
		DrawDebugSphere(
			GetWorld()
			, Origin
			, Hitbox.Radius
			, 8, FColor::White, false, -1.f, (uint8)'\000', 2.f
		);
		DrawDebugLine(
			GetWorld()
			, Origin + (op * Hitbox.Radius)
			, End + (op * Hitbox.Radius)
			, FColor::White, false, -1.f, (uint8)'\000', 2.f
		);
		DrawDebugLine(
			GetWorld()
			, Origin - (op * Hitbox.Radius)
			, End - (op * Hitbox.Radius)
			, FColor::White, false, -1.f, (uint8)'\000', 2.f
		);
		DrawDebugSphere(
			GetWorld()
			, End
			, Hitbox.Radius
			, 8, FColor::White, false, -1.f, (uint8)'\000', 2.f
		);
	}

	return Result;
}


EHitResponse ABBGameMode::HitActor(TWeakObjectPtr<AActor> Actor, EFighterDamageType DamageType, FImpactData& Hit, FImpactData& Block, float Accuracy, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, bool grab, ERPSType RPSType)
{
	if (!Actor.IsValid())
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("GameMode asked to hit an invalid actor by source %s contoller %s."),
			(Source.IsValid()) ? *GetNameSafe(Source.Get()) : TEXT("INVALID SOURCE"),
			(SourceController.IsValid()) ? *GetNameSafe(SourceController.Get()) : TEXT("INVALID CONTROLLER")
		);
		return EHitResponse::HE_Missed;
	}

	AFighterCharacter *Fighter = Cast<AFighterCharacter>(Actor.Get());
	AFighterCharacter *OpponentFighter = Cast<AFighterCharacter>(Source.Get());
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
	


	if (CheckClash(Source, Actor))
	{
		UE_LOG(LogClashing, Log, TEXT("Clash detected between %s and %s."), *Actor.Get()->GetName(), *Source.Get()->GetName());
		int res = OnClash(Source, Actor);

		switch (res) {
		case 1:
			Fighter->StartStun(GetScaledTime(10), false);
			//OpponentFighter->Moveset->Parry();
			break; // The Source won the clash, continue on.
		case 0:
			if (Fighter->GetFighterHitbox().RPSCategory == ERPSType::RPS_Attack)
			{
				return EHitResponse::HE_Clashed;
			}
			else
			{
				return EHitResponse::HE_Missed;
			}
			break;
		case -1:
			OpponentFighter->StartStun(GetScaledTime(10), false);
			//Fighter->Moveset->Parry();
			return EHitResponse::HE_Missed;
			break;
		}
	}

	AfterHitstopSourceController = SourceController;
	AfterHitstopActor = Actor;
	AfterHitstopSource = Source;

	bool WasBlocked = DoesBlock(Fighter, DamageType, RPSType);

	if (Hit.StunLength > 0)
	{
		if (Fighter != nullptr)
		{
			Fighter->StopAnimMontage();
		}
		Fighter->StartStun(GetScaledTime(Hit.StunLength), WasBlocked);
	}

	if (Hit.HitstopAmount > 0) {
		//shake camera as part of hitstop
		UGameplayStatics::PlayWorldCameraShake(this, CameraShake, GetGameState<ABBGameState>()->MainCamera->GetActorLocation(), 0.0f, 500.0f, 1.0f, false);
	}
	if (grab)
	{
		Fighter->Grabbed(Hit.StunLength);
	}
	Fighter->WasHitBPEvents();
	HitstopEvents(DamageType, Hit, Block, Accuracy, Hit.HitstopAmount, Fighter->GetIndex(), RPSType, WasBlocked);

	return (WasBlocked) ? EHitResponse::HE_Blocked : EHitResponse::HE_Hit;
}

void ABBGameMode::K2_OnMatchStart_Implementation()
{
	StartRound();
}

void ABBGameMode::K2_StartRound()
{
	StartRound();
}

void ABBGameMode::HitstopEvents_Implementation(EFighterDamageType DamageType, FImpactData Hit, FImpactData Block, float Accuracy, float HitstopAmount, int OpponentIndex, ERPSType RPSType, bool WasBlocked)
{
	EventsAfterHitstop(DamageType, Hit, Block, Accuracy, RPSType);
}

void ABBGameMode::ParryHitstop_Implementation(int WinnerIndex, AFighterCharacter* winner)
{
	EventsAfterParry(winner);
}

void ABBGameMode::EventsAfterParry(AFighterCharacter* winner)
{
	winner->Moveset->Parry();
}

void ABBGameMode::EventsAfterHitstop(EFighterDamageType DamageType, FImpactData Hit, FImpactData Block, float Accuracy, ERPSType RPSType)
{
	
	IFighter *Fighter = Cast<IFighter>(AfterHitstopActor.Get());
	bool WasBlocked = DoesBlock(Fighter, DamageType, RPSType);


	if (Fighter != nullptr)
	{
		Fighter->AddHit();
	}


	FImpactData* ImpactData = (WasBlocked) ? &Block : &Hit;
	FImpactData ScaledImpact = GetScaledImpactData(AfterHitstopActor.Get(), *ImpactData, Accuracy);

	
	if (ApplyImpact(AfterHitstopActor, ScaledImpact, WasBlocked, AfterHitstopSourceController, AfterHitstopSource) == 1
		&& !ScaledImpact.ImpartedMovement.UsePhysicsLaunch && !Fighter->IsJumping())
	{
		UE_LOG(LogABBGameMode, Verbose, TEXT("%s::HitActor actor backed into wall, applying to source."), *GetNameSafe(this));
		//The target is already pushed up against a wall, push back the source instead.
		//Don't do this for projectiles.
		if (AfterHitstopSource.IsValid() && AfterHitstopSourceController.IsValid()
			&& AfterHitstopSource.Get() == AfterHitstopSourceController.Get()->GetPawn())
		{
			FMovement MovementToAttacker = ImpactData->ImpartedMovement;
			MovementToAttacker.Delta.X *= -1;
			MovementToAttacker.Delta.Y = 0;
			MovementToAttacker.InAirLaunchDelta.X = -100;
			MovementToAttacker.InAirLaunchDelta.Y = 0;
			ApplyMovementToActor(AfterHitstopSource, AfterHitstopSource, AfterHitstopSourceController, MovementToAttacker);
		}
	}
}


int ABBGameMode::GetWinnerIndex()
{
	ABBPlayerState* Player0State = Cast<ABBPlayerState>(GameState->PlayerArray[0]);
	ABBPlayerState* Player1State = Cast<ABBPlayerState>(GameState->PlayerArray[1]);
	if (Player0State != nullptr)
	{
		if (Player1State != nullptr)
		{
			if (Player0State->GetHealth() == Player1State->GetHealth()) return -1;
			return (Player0State->GetHealth() > Player1State->GetHealth()) ? 0 : 1;
		}
		return 0;
	}
	return -1;
}

void ABBGameMode::EndRound()
{
	if (!bIsInRound || bDebugMode)
	{
		return;
	}
	bIsInRound = false;

	UE_LOG(LogBeatBoxers, Log, TEXT("Round ending."));
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_RoundEnd))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_RoundEnd);
	}

	// TODO: put winner of round in DDR state

	int Winner = GetWinnerIndex();

	if (Winner >= 0)
	{
		if (GameState->PlayerArray.Num() > Winner && GameState->PlayerArray[Winner] != nullptr)
		{
			ABBPlayerState *PlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[Winner]);
			if (PlayerState != nullptr)
			{
				++PlayerState->RoundsWon;
				PushMusicBalance();
				if (PlayerState->RoundsWon >= RoundsToWin)
				{
					EndGame(Winner);

					return;
				}
				else
				{
					if (RoundEndEvent.IsBound())
					{
						RoundEndEvent.Broadcast(Winner);
					}
				}
			}
			else
			{
				UE_LOG(LogBeatBoxers, Warning, TEXT("Winner could not be cast."));
			}
		}
		else
		{
			UE_LOG(LogBeatBoxers, Warning, TEXT("Round had invalid winner %i."), Winner);
		}
	}

	StartRoundWithDelay();
}








bool ABBGameMode::IsOnBeat(float Accuracy)
{
	float AccInTime = -1;
	IMusicBox* MusicBox = Cast<IMusicBox>(GetMusicBox());
	if (MusicBox != nullptr)
	{
		AccInTime = (1.f - Accuracy) * MusicBox->GetTimeBetweenBeats();
		if (AccInTime <= AfterBeatAccuracyWindow  || AccInTime >= MusicBox->GetTimeBetweenBeats() - BeforeBeatAccuracyWindow || StillOnBeat)
		{
			UE_LOG(LogBeatTiming, Verbose, TEXT("IsOnBeat(%f) with BeforeBeatAccuracyWindow %f and AfterBeatAccuracyWindow %f? True, AccInTime +%f -%f.")
				, Accuracy
				, BeforeBeatAccuracyWindow
				, AfterBeatAccuracyWindow
				, AccInTime
				, MusicBox->GetTimeBetweenBeats() - AccInTime
			);
			return true;
		}
		UE_LOG(LogBeatTiming, Verbose, TEXT("IsOnBeat(%f) with BeforeBeatAccuracyWindow %f and AfterBeatAccuracyWindow %f? False, AccInTime +%f -%f.")
			, Accuracy
			, BeforeBeatAccuracyWindow
			, AfterBeatAccuracyWindow
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

bool ABBGameMode::DoesBlock(IFighter *Fighter, EFighterDamageType DamageType, ERPSType RPSType) const
{
	if (Fighter == nullptr || !Fighter->IsBlocking()) return false;

	if (RPSType == ERPSType::RPS_Grab) return false;

	return true;
	/*
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
	*/
}

int ABBGameMode::ApplyMovementToActor(TWeakObjectPtr<AActor> Target, TWeakObjectPtr<AActor> Source, TWeakObjectPtr<AController> SourceController, FMovement Movement)
{
	if (!Movement.IsValid() && !Cast<IFighter>(Target.Get())->IsJumping())
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
			NonrelativeMovement.InAirLaunchDelta.X *= Fighter->GetFacing();
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
	ACharacter* Character = Cast<ACharacter>(Target.Get());
	IFighter *TargetFighter = Cast<IFighter>(Target.Get());
	if (Character != nullptr)
	{
		// If you pass a zero vector it doesn't do anything.
		Character->LaunchCharacter(FVector(0.f,0.f,1.f), true, true);
		TargetFighter->ApplyMovement(FMovement{});
	}
	if (!Movement.UsePhysicsLaunch && !TargetFighter->IsJumping())
	{
		if (TargetFighter != nullptr)
		{
			TargetFighter->ApplyMovement(NonrelativeMovement);
		}
	}
	else if (!Movement.UsePhysicsLaunch) //launches character using InAirLaunchDelta if they're in the air and physics launch isnt set to true
	{
		if (Character != nullptr)
		{
			FVector Launch{ NonrelativeMovement.InAirLaunchDelta.X, 0.f, NonrelativeMovement.InAirLaunchDelta.Y };
			Character->LaunchCharacter(Launch, true, true);
		}
	}
	else
	{
		if (Character != nullptr)
		{
			FVector Launch{ NonrelativeMovement.Delta.X, 0.f, NonrelativeMovement.Delta.Y };
			Character->LaunchCharacter(Launch, true, true);
			Cast<AFighterCharacter>(Character)->SetFighterCollisions(false);
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
			MusicBox->GetOnBeatEvent().AddDynamic(this, &ABBGameMode::OnBeat);
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
	UE_LOG(LogABBGameMode, Verbose, TEXT("Music ended, restarting music."));
	IMusicBox *WorldMusicBox = GetGameState<ABBGameState>()->GetIMusicBox();
	if (WorldMusicBox != nullptr)
	{
		UBBGameInstance *GameInstance = GetWorld()->GetGameInstance<UBBGameInstance>();
		if (GameInstance != nullptr)
		{
			WorldMusicBox->StartMusic(GameInstance->NewGameData.SongName, GetMusicBalance());
		}
	}
}

void ABBGameMode::HandleMatchIsWaitingToStart()
{
	bReadyToEnd = false;
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

void ABBGameMode::HandleMatchHasStarted()
{
	GameSession->HandleMatchHasStarted();

	UE_LOG(LogABBGameMode, Log, TEXT("New match started."));

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
			UBBGameInstance *GameInstance = GetWorld()->GetGameInstance<UBBGameInstance>();
			if (GameInstance != nullptr)
			{
				FMusicBalanceParams Params{};
				AFighterCharacter *Fighter;
				Fighter = Cast<AFighterCharacter>(GameInstance->NewGameData.Player0Class.GetDefaultObject());
				if (Fighter != nullptr)
				{
					Params += Fighter->FighterData.MusicBalance;
				}
				Fighter = Cast<AFighterCharacter>(GameInstance->NewGameData.Player1Class.GetDefaultObject());
				if (Fighter != nullptr)
				{
					Params += Fighter->FighterData.MusicBalance;
				}

				WorldMusicBox->StartMusic(GameInstance->NewGameData.SongName, Params);
			}
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
	
	SetPlayerInput(false);

	K2_OnMatchStart();

	// Make sure level streaming is up to date before triggering NotifyMatchStarted
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());

	// First fire BeginPlay, if we haven't already in waiting to start match
	GetWorldSettings()->NotifyBeginPlay();

	// Then fire off match started
	GetWorldSettings()->NotifyMatchStarted();

	// if passed in bug info, send player to right location
	const FString BugLocString = UGameplayStatics::ParseOption(OptionsString, TEXT("BugLoc"));
	const FString BugRotString = UGameplayStatics::ParseOption(OptionsString, TEXT("BugRot"));
	if( !BugLocString.IsEmpty() || !BugRotString.IsEmpty() )
	{
		for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
		{
			APlayerController* PlayerController = Iterator->Get();
			if( PlayerController->CheatManager != nullptr )
			{
				PlayerController->CheatManager->BugItGoString( BugLocString, BugRotString );
			}
		}
	}

	if (IsHandlingReplays() && GetGameInstance() != nullptr)
	{
		GetGameInstance()->StartRecordingReplay(GetWorld()->GetMapName(), GetWorld()->GetMapName());
	}
}

FImpactData ABBGameMode::GetScaledImpactData_Implementation(AActor *Target, const FImpactData& ImpactData, float Accuracy)
{
	if (Target == nullptr)
	{
		return ImpactData;
	}

	float Scale = 1.f;
	IFighter *Fighter = Cast<IFighter>(Target);
	if (Fighter != nullptr)
	{
		int TimesHit = Fighter->GetTimesHitThisKnockdown();
		Scale = FGenericPlatformMath::Pow(ComboImpactScaling, TimesHit);
		//float absolute = Accuracy - 0.5f;
		//if (absolute < 0) {
		//	absolute *= -1;
		//}
		//float scalar = 0.75 + (2 * absolute * absolute);
		FImpactData ScaledImpact = ImpactData;
		ScaledImpact.Damage *= Scale;
		ScaledImpact.ImpartedMovement.Delta *= GlobalImpactScaling;
		if (ScaledImpact.ImpartedMovement.UsePhysicsLaunch)
		{
			ScaledImpact.ImpartedMovement.Delta *= Scale;// * scalar;
		}
		return ScaledImpact;
	}

	return ImpactData;
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

FRoundStartEvent& ABBGameMode::GetOnRoundStartEvent()
{
	return RoundStartEvent;
}

FCombatStartEvent& ABBGameMode::GetOnCombatStartEvent()
{
	return CombatStartEvent;
}

FFightStartedEvent& ABBGameMode::GetOnFightStartedEvent()
{
	return FightStartedEvent;
}

FRoundEndEvent& ABBGameMode::GetOnRoundEndEvent()
{
	return RoundEndEvent;
}

FMatchEndEvent& ABBGameMode::GetOnMatchEndEvent()
{
	return MatchEndEvent;
}

FBeatWindowCloseEvent& ABBGameMode::GetOnBeatWindowCloseEvent()
{
	return BeatWindowCloseEvent;
}

FPlayerBeatComboChangedEvent& ABBGameMode::GetOnPlayerBeatComboChangedEvent()
{
	return PlayerBeatComboChangedEvent;
}

void ABBGameMode::AdjustCamera()
{
	if (!bIsInRound) // Only Adjust camera if still playing.
	{
		return;
	}
	ABBWorldSettings *Settings = Cast<ABBWorldSettings>(GetWorldSettings());
	if (Settings != nullptr)
	{
		FVector CamPos;
		if (GetGameState<ABBGameState>() != nullptr)
		{
			ACameraActor *Cam = GetGameState<ABBGameState>()->MainCamera;
			if (Cam != nullptr)
			{
				CamPos = Cam->GetActorLocation();
			}
		}
		FVector Target = CamPos;
		FVector p1Loc = Settings->InitialCameraLookAtLocation
			, p2Loc = Settings->InitialCameraLookAtLocation;
		if (GameState->PlayerArray.Num() > 0)
		{
			ABBPlayerState *BBPlayerState;
			BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[0]);
			if (BBPlayerState != nullptr && BBPlayerState->MyPawn != nullptr)
			{
				p1Loc = BBPlayerState->MyPawn->GetActorLocation();
			}
		}
		if (GameState->PlayerArray.Num() > 1)
		{
			ABBPlayerState *BBPlayerState;
			BBPlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[1]);
			if (BBPlayerState != nullptr && BBPlayerState->MyPawn != nullptr)
			{
				p2Loc = BBPlayerState->MyPawn->GetActorLocation();
			}
		}

		float xDelta = FMath::Abs(p1Loc.X - p2Loc.X) + 2 * Settings->PlayerCameraPadding.X;
		if (xDelta > Settings->CameraFocus.X)
		{
			Target.X = (p1Loc.X + p2Loc.X) / 2.f;
		}
		else
		{
			float tPos;
			if (FMath::Abs(p1Loc.X - CamPos.X) >= FMath::Abs(p2Loc.X - CamPos.X))
			{
				tPos = p1Loc.X;
			}
			else
			{
				tPos = p2Loc.X;
			}
			if (FMath::Abs(CamPos.X - tPos) + Settings->PlayerCameraPadding.X > Settings->CameraFocus.X / 2.f)
			{
				Target.X = CamPos.X + FMath::Sign(tPos - CamPos.X) * (FMath::Abs(tPos - CamPos.X) + Settings->PlayerCameraPadding.X - (Settings->CameraFocus.X / 2.f));
			}
		}

		float zDelta = FMath::Abs(p1Loc.Z - p2Loc.Z) + 2 * Settings->PlayerCameraPadding.Y;
		if (zDelta > Settings->CameraFocus.Y)
		{
			Target.Z = (p1Loc.Z + p2Loc.Z) / 2.f;
		}
		else
		{
			float tPos;
			if (FMath::Abs(p1Loc.Z - CamPos.Z) >= FMath::Abs(p2Loc.Z - CamPos.Z))
			{
				tPos = p1Loc.Z;
			}
			else
			{
				tPos = p2Loc.Z;
			}
			if (FMath::Abs(tPos - CamPos.Z) + Settings->PlayerCameraPadding.Y > Settings->CameraFocus.Y / 2.f)
			{
				Target.Z += FMath::Sign(tPos - CamPos.Z) * (FMath::Abs(tPos - CamPos.Z) + Settings->PlayerCameraPadding.Y - (Settings->CameraFocus.Y / 2.f));
			}
		}

		Target.X = FMath::Clamp(Target.X, Settings->CameraBounds.Min.X, Settings->CameraBounds.Max.X);
		Target.Z = FMath::Clamp(Target.Z-100, Settings->CameraBounds.Min.Y, Settings->CameraBounds.Max.Y);

		if (GetGameState<ABBGameState>() != nullptr)
		{
			ACameraActor *Cam = GetGameState<ABBGameState>()->MainCamera;
			if (Cam != nullptr)
			{				
				Cam->SetActorLocation(Target);
			}
		}
	}
}

void ABBGameMode::Tick(float DeltaSeconds)
{
	AGameMode::Tick(DeltaSeconds);

	AdjustCamera();

}



void ABBGameMode::StartRoundWithDelay()
{
	if (DelayBetweenRounds > 0)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_StartNextRound
			, this
			, &ABBGameMode::StartRound
			, DelayBetweenRounds
			, false
		);
	}
	else
	{
		StartRound();
	}
}

void ABBGameMode::StartRound()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("Round starting."));
	bIsInRound = true;
	RoundNumber++;
	for (int i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ABBPlayerState* PlayerState = Cast<ABBPlayerState>(GameState->PlayerArray[i]);
		if (PlayerState != nullptr)
		{
			PlayerState->ResetPlayerState();
		}
	}
	SpawnPawns();
	SetPlayerInput(false);
	GetWorldTimerManager().SetTimer(
		TimerHandle_StartCombat
		, this
		, &ABBGameMode::StartCombat
		, DelayBeforeCombat
		, false
	);
	if (RoundStartEvent.IsBound())
	{
		RoundStartEvent.Broadcast();
	}
}

void ABBGameMode::StartCombat()
{
	SetPlayerInput(true);
	if (RoundTime <= 0)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("Game mode round time is invalid, proceeding with 90s."));
		RoundTime = 90;
	}
	GetWorldTimerManager().SetTimer(
		TimerHandle_RoundEnd
		, this
		, &ABBGameMode::OnRoundTimeOut
		, RoundTime
		, false
	);
	GetWorldTimerManager().SetTimer(
		TimerHandle_FightStarted
		, this
		, &ABBGameMode::StartedFight
		, FightStartTime
		, false
	);
	if (CombatStartEvent.IsBound())
	{
		CombatStartEvent.Broadcast();
	}
}

void ABBGameMode::StartedFight()
{
	if (FightStartedEvent.IsBound())
	{
		FightStartedEvent.Broadcast();
	}
}

void ABBGameMode::OnRoundTimeOut()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("Round time has run out."));
	EndRound();
}

float ABBGameMode::GetTimeLeftInRound()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_RoundEnd))
	{
		return GetWorldTimerManager().GetTimerRemaining(TimerHandle_RoundEnd);
	}
	else
	{
		return 0;
	}
}

void ABBGameMode::SetPlayerInput(bool IsEnabled)
{
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController->GetPawn() != nullptr)
		{
			if (IsEnabled)
			{
				PlayerController->GetPawn()->EnableInput(PlayerController);
			}
			else
			{
				PlayerController->GetPawn()->DisableInput(PlayerController);
			}
		}
	}
}

float ABBGameMode::TimeToNextRound()
{
	if (!bIsInRound && GetWorldTimerManager().IsTimerActive(TimerHandle_StartNextRound))
	{
		return GetWorldTimerManager().GetTimerRemaining(TimerHandle_StartNextRound);
	}
	return 0;
}

void ABBGameMode::AttachOpponents()
{
	if (GameState == nullptr)
	{
		UE_LOG(LogABBGameMode, Error, TEXT("GameState null when attach opponents was called."));
		return;
	}
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

float ABBGameMode::GetScaledTime(float time) 
{
	IMusicBox* MusicBox = Cast<IMusicBox>(GetMusicBox());
	float TimeBetweenBeats = MusicBox->GetTimeBetweenBeats();

	return (TimeBetweenBeats*time);
}

void ABBGameMode::SpawnPawns()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController->GetPawn() != nullptr)
		{
			APawn *Pawn = PlayerController->GetPawn();
			PlayerController->UnPossess();
			Pawn->Destroy();
		}
	}
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if ((PlayerController->GetPawn() == nullptr) && PlayerCanRestart(PlayerController))
		{
			RestartPlayer(PlayerController);
		}
	}
	AttachOpponents();
}

bool ABBGameMode::ReadyToEndMatch_Implementation()
{
	return bReadyToEnd;
}

void ABBGameMode::EndGame(int Winner)
{
	UE_LOG(LogBeatBoxers, Log, TEXT("Match ending."));
	if (MatchEndEvent.IsBound())
	{
		MatchEndEvent.Broadcast(Winner);
	}
	bReadyToEnd = true;
	RoundNumber = 0;
}

void ABBGameMode::HandleMatchHasEnded()
{
	if (DelayBeforeEnd > 0)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_StartNextRound
			, this
			, &ABBGameMode::RestartGame
			, DelayBeforeEnd
			, false
		);
	}
	else
	{
		RestartGame();
	}
}

FMusicBalanceParams ABBGameMode::GetMusicBalance()
{
	FMusicBalanceParams FinalParams;
	if (GameState == nullptr || GetMusicBox() == nullptr || GetGameInstance() == nullptr) return FinalParams;
	IMusicBox *MusicBox = Cast<IMusicBox>(GetMusicBox());
	UBBGameInstance *GameInstance = Cast<UBBGameInstance>(GetGameInstance());
	if (GameState->PlayerArray.Num() < 2 || MusicBox == nullptr || GameInstance == nullptr) return FinalParams;
	ABBPlayerState *PlayerStates[2];
	AFighterCharacter *DefaultFighter[2];
	FMusicBalanceParams MusicParams[2];
	DefaultFighter[0] = Cast<AFighterCharacter>(GameInstance->NewGameData.Player0Class.GetDefaultObject());
	DefaultFighter[1] = Cast<AFighterCharacter>(GameInstance->NewGameData.Player1Class.GetDefaultObject());
	for (int i = 0; i < 2; i++)
	{
		PlayerStates[i] = Cast<ABBPlayerState>(GameState->PlayerArray[i]);
		if (PlayerStates[i] == nullptr) return FinalParams;
		if (DefaultFighter[i] != nullptr)
		{
			MusicParams[i] = DefaultFighter[i]->GetMusicBalance();
		}
	}
	int r0 = PlayerStates[0]->RoundsWon;
	int r1 = PlayerStates[1]->RoundsWon;
	if (r0 == r1)
	{
		FinalParams = MusicParams[0] + MusicParams[1];
	}
	else if (r0 > r1)
	{
		FinalParams = MusicParams[0];
	}
	else
	{
		FinalParams = MusicParams[1];
	}
	return FinalParams;
}

void ABBGameMode::PushMusicBalance()
{
	if (GetMusicBox() == nullptr) return;
	IMusicBox *MusicBox = Cast<IMusicBox>(GetMusicBox());
	MusicBox->ChangeBalance(GetMusicBalance());
}

bool ABBGameMode::CheckClash(TWeakObjectPtr<AActor> ActorA, TWeakObjectPtr<AActor> ActorB)
{
	if (ActorA.IsValid() && ActorB.IsValid())
	{
		AFighterCharacter* mFighterA = Cast<AFighterCharacter>(ActorA.Get());
		AFighterCharacter* mFighterB = Cast<AFighterCharacter>(ActorB.Get());

		if (mFighterA != nullptr && mFighterB != nullptr
			&& mFighterA->CanClash() && mFighterB->CanClash())
		{
			TArray<TWeakObjectPtr<AActor>> ActorsToIgnore;
			ActorsToIgnore.Add(ActorA);
			FMoveHitbox Hitbox;
			Hitbox.Radius = mFighterA->GetFighterHitbox().Radius;
			Hitbox.Origin = mFighterA->GetFighterHitbox().Origin;
			Hitbox.End = mFighterA->GetFighterHitbox().End;

			//Make hitboxes relative to facing
			Hitbox.Origin.X *= mFighterA->GetFacing();
			Hitbox.End.X *= mFighterA->GetFacing();

			FHitResult HitResult = TraceHitbox(
				ActorA.Get()->GetActorLocation(),
				Hitbox,
				ActorsToIgnore
			);
			
			if (HitResult.bBlockingHit && HitResult.Actor.IsValid())
			{
				ActorsToIgnore.Empty();
				ActorsToIgnore.Add(ActorB);
				Hitbox.Radius = mFighterB->GetFighterHitbox().Radius;
				Hitbox.Origin = mFighterB->GetFighterHitbox().Origin;
				Hitbox.End = mFighterB->GetFighterHitbox().End;

				//Make hitboxes relative to facing
				Hitbox.Origin.X *= mFighterB->GetFacing();
				Hitbox.End.X *= mFighterB->GetFacing();

				HitResult = TraceHitbox(
					ActorB.Get()->GetActorLocation(),
					Hitbox,
					ActorsToIgnore
				);
				if (HitResult.bBlockingHit && HitResult.Actor.IsValid())
				{
					Cast<UFighterStateComponent>(mFighterA->GetFighterState())->PlayExecutionAnimation();
					Cast<UFighterStateComponent>(mFighterB->GetFighterState())->PlayExecutionAnimation();
					return true;
				}
			}
			

		}
	}
	return false;
}

int ABBGameMode::OnClash(TWeakObjectPtr<AActor> FighterA, TWeakObjectPtr<AActor> FighterB)
{
	AFighterCharacter* mFighterA = Cast<AFighterCharacter>(FighterA.Get());
	AFighterCharacter* mFighterB = Cast<AFighterCharacter>(FighterB.Get());

	if (mFighterA != nullptr && mFighterB != nullptr)
	{
		IFighter* winner = DetermineClashWinner(mFighterA, mFighterB);
		// Passes the fighters themselves as the source so the clash impact will be relative to their facing.

		FTransform ImpactTransform = FTransform::Identity;
		ImpactTransform.SetTranslation((FighterA.Get()->GetActorLocation() + FighterB.Get()->GetActorLocation()) / 2.f);
		FTransform RelativeTransform = DefaultClashImpact.SFX.RelativeTransform * ImpactTransform;

		if (winner == nullptr)
		{
			if (mFighterA->GetFighterHitbox().RPSCategory == ERPSType::RPS_Attack)
			{
				ApplyImpact(FighterA, GetClashImpact(mFighterA == winner), false, nullptr, FighterA);
				ApplyImpact(FighterB, GetClashImpact(mFighterB == winner), false, nullptr, FighterB);
				mFighterA->Clash();
				mFighterB->Clash();



				if (DefaultClashImpact.SFX.ParticleSystem != nullptr)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						DefaultClashImpact.SFX.ParticleSystem,
						RelativeTransform
					);
				}
				if (DefaultClashImpact.SFX.SoundCue != nullptr)
				{
					UGameplayStatics::SpawnSoundAtLocation(
						GetWorld(),
						DefaultClashImpact.SFX.SoundCue,
						RelativeTransform.GetLocation(),
						RelativeTransform.GetRotation().Rotator()
					);
				}
			}
		}
		else if (winner->GetFighterHitbox().RPSCategory == ERPSType::RPS_Block){
			ParryHitstop(winner->GetIndex(), Cast<AFighterCharacter>(winner));//EventsAfterparry
			if (ParryClashImpact.SFX.SoundCue != nullptr)
			{
				UGameplayStatics::SpawnSoundAtLocation(
					GetWorld(),
					ParryClashImpact.SFX.SoundCue,
					RelativeTransform.GetLocation(),
					RelativeTransform.GetRotation().Rotator()
				);
			}
		}


		if (winner == mFighterA) 
			return 1; 
		if (winner == mFighterB) 
			return -1;
		return 0;
	}
	else
	{
		UE_LOG(LogClashing, Warning, TEXT("ABBGameMode::OnClash given a nullptr for a Fighter."));
	}
	return 0;
}

IFighter* ABBGameMode::DetermineClashWinner(IFighter* FighterA, IFighter* FighterB)
{
	ERPSType FighterA_RPSCategory = FighterA->GetFighterHitbox().RPSCategory;
	ERPSType FighterB_RPSCategory = FighterB->GetFighterHitbox().RPSCategory;

	if (FighterA_RPSCategory == ERPSType::RPS_None || FighterB_RPSCategory == ERPSType::RPS_None)
	{
		UE_LOG(LogClashing, Warning, TEXT("ABBGameMode::DetermineClashWinner given a None for a RPSCategory for one or more fighters."));
	}
	//if (FighterA_RPSCategory == ){
	//	UE_LOG(LogClashing, Warning, TEXT("player 1 %s"));
	//}

	int winner = GetRPSWinner(FighterA_RPSCategory, FighterB_RPSCategory);
	switch (winner)
	{
	case 1:
		return FighterA;
	case 2:
		return FighterB;
	default:
		return nullptr;
	}
}

int ABBGameMode::ApplyImpact(TWeakObjectPtr<AActor> Actor, FImpactData ImpactData, bool WasBlocked, TWeakObjectPtr<AController> SourceController, TWeakObjectPtr<AActor> Source)
{
	int toRet = ApplyMovementToActor(Actor, Source, SourceController, ImpactData.ImpartedMovement);


	IFighter* Fighter = Cast<IFighter>(Actor.Get());
	if (Fighter != nullptr)
	{

		if ((ImpactData.bKnocksDown && !WasBlocked) || Fighter->IsJumping())
		{
			IFighter* Fighter = Cast<IFighter>(Actor.Get());
			if (Fighter != nullptr)
			{
				Fighter->Knockdown();
			}
		}
		else {
		}
	}
	
	if (SourceController.IsValid() && SourceController.Get()->PlayerState != nullptr)
	{
		AddSpecial(SourceController.Get()->PlayerState, ImpactData.SpecialGenerated);
	}

	// Logic for applying damage and giving special to opponent
	APawn* mAPawn = Cast<APawn>(Actor.Get());
	if (mAPawn != nullptr && mAPawn->Controller != nullptr)
	{
		APlayerController* mPlayerController = Cast<APlayerController>(mAPawn->Controller);
		if (mPlayerController != nullptr && mPlayerController->PlayerState != nullptr)
		{
			ABBPlayerState* mBBPlayerState = Cast<ABBPlayerState>(mPlayerController->PlayerState);
			if (mBBPlayerState != nullptr)
			{
				mBBPlayerState->TakeDamage(ImpactData.Damage * GlobalDamageScaling);
				AddSpecial(mBBPlayerState, ImpactData.SpecialGenerated / 4.0);
				if (mBBPlayerState->GetHealth() == 0)
				{
					UE_LOG(LogBeatBoxers, Log, TEXT("A player has died."));
					EndRound();
				}
			}
		}
	}
	return toRet;
}

void ABBGameMode::OnBeat()
{
	StillOnBeat = true;
	if (AfterBeatAccuracyWindow > 0)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_BeatWindowClose
			, this
			, &ABBGameMode::BeatWindowClose
			, AfterBeatAccuracyWindow
		);
	}
	else
	{
		BeatWindowClose();
	}
}

void ABBGameMode::BeatWindowClose()
{
	StillOnBeat = false;
	if (BeatWindowCloseEvent.IsBound())
	{
		BeatWindowCloseEvent.Broadcast();
	}
}
