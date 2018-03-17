// copyright 2017 BYU Animation

#include "MovesetComponent.h"
#include "../BBGameMode.h"
#include "GameFramework/Actor.h"
#include "FighterCharacter.h"
#include "../BBGameState.h"


// Sets default values for this component's properties
UMovesetComponent::UMovesetComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BufferAccuracy = -1.f;
}


// Called when the game starts
void UMovesetComponent::BeginPlay()
{
	Super::BeginPlay();
	cs = GetNameSafe(this);

	if (MyFighter != nullptr)
	{
		DefaultState = MyFighter->GetDefaultMoveState();
	}
	GotoDefaultState();
}

void UMovesetComponent::OnPostWaitExpired()
{
	GotoDefaultState();
}

void UMovesetComponent::GotoDefaultState()
{
	if (DefaultState.GetRow<FMoveData>(cs) == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("%s UMovesetComponent has no valid default move state!"), *GetNameSafe(GetOwner()));
		return;
	}
	SetState(DefaultState);
}

void UMovesetComponent::SetState(FDataTableRowHandle State)
{
	Cast<AFighterCharacter>(MyFighter)->K2_BPEventsOnStateBegin(State);
	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent changing state to %s"), *GetNameSafe(GetOwner()), *State.RowName.ToString());
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_PostWait))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_PostWait);
	}
	PreviousState = CurrentState;
	CurrentState = State;
	CurrentWindowInState = 0;

}

void UMovesetComponent::GotoState(FDataTableRowHandle NewState)
{
	Cast<AFighterCharacter>(MyFighter)->HasUsedMoveAndHasYetToLand = true;

	

	if (!(Cast<AFighterCharacter>(MyFighter)->InAir
		|| Cast<AFighterCharacter>(MyFighter)->FighterState != nullptr && (Cast<AFighterCharacter>(MyFighter)->FighterState->IsMidMove() || Cast<AFighterCharacter>(MyFighter)->FighterState->IsStunned())))
	{
		Cast<AFighterCharacter>(MyFighter)->UpdateFacing();
	}


	if (NewState.GetRow<FMoveData>(cs) == nullptr)
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent passed invalid class reference as new state."), *GetNameSafe(GetOwner()));
		GotoDefaultState();
	}
	else
	{
		SetState(NewState);
		//Requiring the last move window to have hit on the first window is nonsensical.
		//Its also currently the only use of this parameter, so just pass true.
		StartNextWindow(true);
	}
}

void UMovesetComponent::StartNextWindow(bool LastWindowHit)
{
	if (CurrentState.GetRow<FMoveData>(cs) == nullptr)
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent CurrentStateClass invalid when trying to start next window."), *GetNameSafe(GetOwner()));
		return;
	}
	bool RequiresLastWindowHit = 
		(CurrentState.GetRow<FMoveData>(cs)->MoveWindows.Num() > CurrentWindowInState)
		? CurrentState.GetRow<FMoveData>(cs)->MoveWindows[CurrentWindowInState].RequiresLastWindowHit
		: false;
	FMoveWindow *NextWindow =
		(CurrentState.GetRow<FMoveData>(cs)->MoveWindows.Num() > CurrentWindowInState) ?
		&CurrentState.GetRow<FMoveData>(cs)->MoveWindows[CurrentWindowInState++] :
		nullptr;

	if (NextWindow == nullptr || (!LastWindowHit && RequiresLastWindowHit))
	{
		// We've reached the end of this window list for this state.
		if (CurrentState.GetRow<FMoveData>(cs)->MaxPostWait == 0)
		{
			// End this state immediately.
			GotoDefaultState();
		}
		else if (CurrentState.GetRow<FMoveData>(cs)->MaxPostWait > 0)
		{
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_PostWait,
				this,
				&UMovesetComponent::OnPostWaitExpired,
				CurrentState.GetRow<FMoveData>(cs)->MaxPostWait,
				false
			);
		}
		else
		{
			// Stay in this state indefinitely.
		}

		// Inform the input parser that this move has finished and we are ready for more input.
		if (MyInputParser != nullptr)
		{
			if (MyFighter != nullptr)
			{
				if (MyFighter->GetStance() != EStance::SE_Jumping)
				{
					MyFighter->UpdateFacing();
				}
			}
			return MyInputParser->OnControlReturned();
		}
		else
		{
			UE_LOG(LogUMoveset, Warning, TEXT("%s UMovesetComponent does not have a valid InputParser reference."))
		}
	}
	else
	{
		if (MyFighterState != nullptr)
		{
			MyFighterState->StartMoveWindow(*NextWindow, MoveAccuracy);
		}
	}
}

void UMovesetComponent::RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld)
{
	if (!FighterWorld.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UMovesetComponent given invalid object to register as FighterWorld."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterWorld = Cast<IFighterWorld>(FighterWorld.Get());
		if (MyFighterWorld == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UMovesetComponent given %s to register as FighterWorld, but it doesn't implement IFighterWorld."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterWorld.Get()));
		}
	}
}

void UMovesetComponent::RegisterFighter(TWeakObjectPtr<UObject> Fighter)
{
	if (!Fighter.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UMovesetComponent given invalid object to register as Fighter."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighter = Cast<IFighter>(Fighter.Get());
		if (MyFighter == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UMovesetComponent given %s to register as Fighter, but it doesn't implement IFighter."), *GetNameSafe(GetOwner()), *GetNameSafe(Fighter.Get()));
		}
	}
}

void UMovesetComponent::RegisterFighterState(TWeakObjectPtr<UObject> FighterState)
{
	if (!FighterState.IsValid())
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given invalid object to register as FighterState."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(FighterState.Get());
		if (MyFighterState == nullptr)
		{
			UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given %s to register as FighterState, but it doesn't implement IFighterState."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterState.Get()));
		}
	}
}

void UMovesetComponent::RegisterInputParser(TWeakObjectPtr<UObject> InputParser)
{
	if (!InputParser.IsValid())
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given invalid object to register as InputParser."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyInputParser = Cast<IInputParser>(InputParser.Get());
		if (MyInputParser == nullptr)
		{
			UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given %s to register as InputParser, but it doesn't implement IInputParser."), *GetNameSafe(GetOwner()), *GetNameSafe(InputParser.Get()));
		}
	}
}

void UMovesetComponent::RegisterSoloTracker(TWeakObjectPtr<UObject> SoloTracker)
{
	if (!SoloTracker.IsValid())
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given invalid object to register as SoloTracker."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MySoloTracker = Cast<ISoloTracker>(SoloTracker.Get());
		if (MySoloTracker == nullptr)
		{
			UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given %s to register as SoloTracker, but it doesn't implement ISoloTracker."), *GetNameSafe(GetOwner()), *GetNameSafe(SoloTracker.Get()));
		}
	}
}

void UMovesetComponent::RegisterMusicBox(TWeakObjectPtr<UObject> MusicBox)
{
	UnbindMusicBox(MyMusicBox);
	if (!MusicBox.IsValid())
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given invalid object to register as MusicBox."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyMusicBox = Cast<IMusicBox>(MusicBox.Get());
		if (MyMusicBox == nullptr)
		{
			UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent given %s to register as MusicBox, but it doesn't implement IMusicBox."), *GetNameSafe(GetOwner()), *GetNameSafe(MusicBox.Get()));
		}
		else
		{
			BindMusicBox(MyMusicBox);
		}
	}
}

void UMovesetComponent::ReceiveInputToken(FBufferInputToken Token)
{
	bool diff = BufferToken != Token.token;
	BufferToken = Token.token;
	BufferAccuracy = Token.accuracy;
	MoveAccuracy = BufferAccuracy;
	ABBGameMode* mGame = (ABBGameMode*)GetWorld()->GetAuthGameMode();
	if (mGame != nullptr)
	{
		if (mGame->IsInRound())
		{
			ProcessInputToken(Token.token, Token.accuracy);
		}
		else
		{
			ProcessDDRInputToken(Token.token);
		}
	}
		

	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent received input token %s with accuracy %f"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>("EInputToken", Token.token), BufferAccuracy);
	UE_LOG(LogBeatTiming, VeryVerbose, TEXT("%s UMovesetComponent recieved input token %s with accuracy %f"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>("EInputToken", Token.token), BufferAccuracy);
	if (MyFighter != nullptr && diff)
	{
		MyFighter->OnInputReceived();
	}
}

void UMovesetComponent::ProcessDDRInputToken(EInputToken Token)
{
	if (BGFretboard == nullptr)
		return;
	FFretboardInputResult BGResult = GetBGFretboard()->ReceiveInputToken(Token);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Hit on the beat of %f"), BGResult.Accuracy));
	if (BGResult.Accuracy > 0.85)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HIT!!"));
		//UGameplayStatics::SpawnEmitterAtLocation(MyFighter, EmitterTemplate, Cast<ACharacter>(MyFighter)->GetActorLocation(), Cast<ACharacter>(MyFighter)->GetActorRotation());
		MyFighter->InputOnBeatLogic();
	}
}

void UMovesetComponent::ProcessInputToken(EInputToken Token, float Accuracy)
{
	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent processing input token %s with accuracy %f"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>("EInputToken", Token), Accuracy);
	if (CurrentState.GetRow<FMoveData>(cs) == nullptr)
	{
		GotoDefaultState();
		return ProcessInputToken(Token, Accuracy);
	}

	FDataTableRowHandle StateToLookAt = CurrentState;

	if ((CurrentState == DefaultState) && (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_PostWait)))
	{
		StateToLookAt = PreviousState;
	}

	for (int i = 0; i < StateToLookAt.GetRow<FMoveData>(cs)->PossibleTransitions.Num(); i++)
	{

		FDataTableRowHandle PossibleMove = StateToLookAt.GetRow<FMoveData>(cs)->PossibleTransitions[i];

		//if in current state and PostWait timer still going on, check PreviousState transistions first
		if ((StateToLookAt == DefaultState) && (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_PostWait)))
		{
			PossibleMove = PreviousState.GetRow<FMoveData>(cs)->PossibleTransitions[i];
		}

		if (PossibleMove.GetRow<FMoveData>(cs) == nullptr)
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("Data Error: Move %s contains invalid state as a possible transition."), *StateToLookAt.RowName.ToString());
		}
		else if (PossibleMove.GetRow<FMoveData>(cs)->AllowedInputs.FilterInputToken(Token))
		{
			if (MyFighterState != nullptr && PossibleMove.GetRow<FMoveData>(cs)->StanceFilter.FilterStance(MyFighterState->GetStance()))
			{
				if (!PossibleMove.GetRow<FMoveData>(cs)->bRequiresOnBeat || MyFighterWorld != nullptr && MyFighterWorld->IsOnBeat(Accuracy))
				{
					if (MyFighterState->UseSpecial(PossibleMove.GetRow<FMoveData>(cs)->SpecialCost))
					{
						// Found a state that we can enter.
						UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent transitioning from state %s to state %s on input %s."),
							*GetNameSafe(GetOwner()),
							*StateToLookAt.RowName.ToString(),
							*StateToLookAt.GetRow<FMoveData>(cs)->PossibleTransitions[i].RowName.ToString(),
							*GetEnumValueToString<EInputToken>(TEXT("EInputToken"), Token)
						);
						MyFighter->SetAttackedThisBeat(true);
						if (MyFighterWorld->IsOnBeat(Accuracy) )
						{
							MyFighter->InputOnBeatLogic();
						}
						else
						{
							MyFighter->MissBeat();
							MyFighter->InputOffBeatLogic();
						}
						GotoState(PossibleMove);
						return;
					}
				}
			}
		}
	}
	// We weren't able to use any of the possible transitions or there were none to begin with.

	if (StateToLookAt != DefaultState)
	{
		// Reached the end of this combo, return to default and try this input again.
		GotoDefaultState();
		return ProcessInputToken(Token, Accuracy);
	}
	else if ((GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_PostWait)))
	{
		//try again but with postwait ended
		(GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_PostWait));
		return ProcessInputToken(Token, Accuracy);
	}
	else
	{
		// If we're at default already there are no moves for this input. Do nothing.
		return;
	}
}

void UMovesetComponent::OnWindowFinished(EWindowEnd WindowEnd)
{
	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent window ended %s"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EWindowEnd>(TEXT("EWindowEnd"), WindowEnd));
	switch (WindowEnd)
	{
	case EWindowEnd::WE_LandSkip:
		GotoDefaultState();
		MyInputParser->OnControlReturned();
		break;
	case EWindowEnd::WE_Missed:
		StartNextWindow(false);
		break;
	case EWindowEnd::WE_Hit:
		StartNextWindow(true);
		break;
	case EWindowEnd::WE_Stunned:
	case EWindowEnd::WE_LandInt:
		GotoDefaultState();
		MyInputParser->OnControlReturned();
		break;
	default:
		break;
	}
}

void UMovesetComponent::OnSoloStart()
{
	//TODO
	if (MySoloTracker != nullptr)
	{
		MySoloTracker->BeginSolo(FSoloParameters());
		//Still need to set everything up to realize we're doing the solo
	}
	//Eventually solo needs to end, probably won't be done in this function.
	if (MyFighterState != nullptr)
	{
		MyFighterState->EndSolo();
	}
}

UBasicFretboard* UMovesetComponent::GetBGFretboard()
{
	if (BGFretboard == nullptr)
	{
		BGFretboard = NewObject<UBasicFretboard>();
		BGFretboard->SetTimerManager(GetOwner()->GetWorldTimerManager());
		ABBGameState *GameState = GetWorld()->GetGameState<ABBGameState>();
		if (GameState != nullptr)
		{
			BGFretboard->Listen(GameState->GetUMusicBox());
		}
		else
		{
			UE_LOG(LogFretboard, Warning, TEXT("%s UMovesetComponent unable reach gamestate to attach fretboard to bg music."), *GetNameSafe(GetOwner()));
		}
	}
	return BGFretboard;
}

UBasicFretboard* UMovesetComponent::GetSoloFretboard()
{
	if (SoloFretboard == nullptr)
	{
		SoloFretboard = NewObject<UBasicFretboard>();
		SoloFretboard->SetTimerManager(GetOwner()->GetWorldTimerManager());
	}
	return SoloFretboard;
}

void UMovesetComponent::BindMusicBox(IMusicBox *MusicBox)
{
}

void UMovesetComponent::UnbindMusicBox(IMusicBox *MusicBox)
{
}

void UMovesetComponent::OnBeat()
{
}

int UMovesetComponent::GetCurrentWindowInMove() const
{
	return CurrentWindowInState - 1;
}

int UMovesetComponent::GetWindowsInCurrentMove() const
{
	return CurrentState.GetRow<FMoveData>(cs)->MoveWindows.Num();
}