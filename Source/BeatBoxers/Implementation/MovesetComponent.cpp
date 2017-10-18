// copyright 2017 BYU Animation

#include "MovesetComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UMovesetComponent::UMovesetComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultStateClass = AMoveState::StaticClass();
}


// Called when the game starts
void UMovesetComponent::BeginPlay()
{
	Super::BeginPlay();
	if (MyFighterState != nullptr)
	{
		DefaultStateClass = MyFighterState->GetDefaultMoveState();
	}
	else
	{
		DefaultStateClass = TSubclassOf<AMoveState>(AMoveState::StaticClass());
	}
	GotoDefaultState();
}

void UMovesetComponent::OnPostWaitExpired()
{
	GotoDefaultState();
}

void UMovesetComponent::GotoDefaultState()
{
	if (DefaultStateClass.Get() == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("%s UMovesetComponent has no valid default class!"), *GetNameSafe(GetOwner()));
		return;
	}
	SetState(DefaultStateClass);
}

void UMovesetComponent::SetState(TSubclassOf<AMoveState> State)
{
	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent changing state to %s"), *GetNameSafe(GetOwner()), *GetNameSafe(State.Get()));
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_PostWait))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_PostWait);
	}
	CurrentStateClass = State;
	CurrentWindowInState = 0;
}

void UMovesetComponent::GotoState(TSubclassOf<AMoveState> NewState)
{
	if (NewState.Get() == nullptr)
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent passed invalid class reference as new state."), *GetNameSafe(GetOwner()));
		GotoDefaultState();
	}
	else
	{
		SetState(NewState);
		StartNextWindow();
	}
}

void UMovesetComponent::StartNextWindow()
{
	if (CurrentStateClass.Get() == nullptr)
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent CurrentStateClass invalid when trying to start next window."), *GetNameSafe(GetOwner()));
		return;
	}
	FMoveWindow *NextWindow =
		(CurrentStateClass.GetDefaultObject()->MoveWindows.Num() > CurrentWindowInState) ?
		&CurrentStateClass.GetDefaultObject()->MoveWindows[CurrentWindowInState++] :
		nullptr;

	if (NextWindow == nullptr)
	{
		// We've reached the end of this window list for this state.
		if (CurrentStateClass.GetDefaultObject()->MaxPostWait == 0)
		{
			// End this state immediately.
			GotoDefaultState();
		}
		else if (CurrentStateClass.GetDefaultObject()->MaxPostWait > 0)
		{
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_PostWait,
				this,
				&UMovesetComponent::OnPostWaitExpired,
				CurrentStateClass.GetDefaultObject()->MaxPostWait,
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
			MyFighterState->StartMoveWindow(*NextWindow);
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

void UMovesetComponent::ReceiveInputToken(EInputToken Token)
{
	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent received input token %s"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>("EInputToken", Token));
	if (CurrentStateClass.Get() == nullptr)
	{
		GotoDefaultState();
		return ReceiveInputToken(Token);
	}

	for (int i = 0; i < CurrentStateClass.GetDefaultObject()->PossibleTransitions.Num(); i++)
	{
		TSubclassOf<AMoveState> PossibleMove = CurrentStateClass.GetDefaultObject()->PossibleTransitions[i];
		if (PossibleMove.GetDefaultObject()->AllowedInputs.FilterInputToken(Token))
		{
			if (MyFighterState != nullptr && PossibleMove.GetDefaultObject()->StanceFilter.FilterStance(MyFighterState->GetStance()))
			{
				if (MyFighterState->UseSpecial(PossibleMove.GetDefaultObject()->SpecialCost))
				{
					// Found a state that we can enter.
					UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent transitioning from state %s to state %s on input %s."),
						*GetNameSafe(GetOwner()),
						*GetNameSafe(CurrentStateClass),
						*GetNameSafe(CurrentStateClass.GetDefaultObject()->PossibleTransitions[i]),
						*GetEnumValueToString<EInputToken>(TEXT("EInputToken"), Token)
					);
					GotoState(CurrentStateClass.GetDefaultObject()->PossibleTransitions[i]);
					return;
				}
			}
		}
	}
	// We weren't able to use any of the possible transitions or there were none to begin with.

	if (CurrentStateClass != DefaultStateClass)
	{
		// Reached the end of this combo, return to default and try this input again.
		GotoDefaultState();
		return ReceiveInputToken(Token);
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
	case EWindowEnd::WE_Finished:
		StartNextWindow();
		break;
	case EWindowEnd::WE_Stunned:
	case EWindowEnd::WE_LandInt:
		GotoDefaultState();
		break;
	default:
		break;
	}
}