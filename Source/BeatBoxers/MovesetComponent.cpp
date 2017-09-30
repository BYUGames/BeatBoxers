// copyright 2017 BYU Animation

#include "MovesetComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UMovesetComponent::UMovesetComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultState = AMoveState::StaticClass();
}


// Called when the game starts
void UMovesetComponent::BeginPlay()
{
	Super::BeginPlay();
	if (MyFighterState == nullptr)
	{
		DefaultState = TSubclassOf<AMoveState>(AMoveState::StaticClass());
	}
	else
	{
		DefaultState = MyFighterState->GetDefaultMoveState();
	}
	GotoDefaultState();
}

AMoveState* UMovesetComponent::GetCurrentState()
{
	if (CurrentState == nullptr)
	{
		GotoState(DefaultState.Get());
	}

	return CurrentState;
}

void UMovesetComponent::OnPostWaitExpired()
{
	GotoDefaultState();
}

void UMovesetComponent::GotoDefaultState()
{
	if (DefaultState.Get() == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Fatal, TEXT("%s UMovesetComponent has no valid default class!"), *GetNameSafe(GetOwner()));
		return;
	}
	SetState(DefaultState);
}

void UMovesetComponent::SetState(TSubclassOf<AMoveState> State)
{
	UE_LOG(LogUMoveset, Verbose, TEXT("%s UMovesetComponent changing state to %s"), *GetNameSafe(GetOwner()), *GetNameSafe(State.Get()));
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_PostWait))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_PostWait);
	}
	if (CurrentState != nullptr && !CurrentState->IsPendingKill())
	{
		CurrentState->MarkPendingKill();
	}
	CurrentState = Cast<AMoveState>(GetWorld()->SpawnActor(State.Get(), &FVector::ZeroVector, &FRotator::ZeroRotator, FActorSpawnParameters()));
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
		if (!NewState.Get()->IsChildOf(AMoveState::StaticClass()))
		{
			UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent passed a class reference that does not inherit from AMoveState."), *GetNameSafe(GetOwner()));
			GotoDefaultState();
		}
		else
		{
			SetState(NewState);
			StartNextWindow();
		}
	}
}

void UMovesetComponent::StartNextWindow()
{
	if (CurrentState == nullptr || CurrentState->IsPendingKill())
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UMovesetComponent CurrentState invalid when trying to start next window."), *GetNameSafe(GetOwner()));
		return;
	}
	FMoveWindow *NextWindow = CurrentState->GetNextWindow();

	if (NextWindow == nullptr)
	{
		// We've reached the end of this window list for this state.
		if (CurrentState->MaxDuration == 0)
		{
			// End this state immediately.
			GotoDefaultState();
		}
		else if (CurrentState->MaxDuration > 0)
		{
			GetOwner()->GetWorldTimerManager().SetTimer(
				TimerHandle_PostWait,
				this,
				&UMovesetComponent::OnPostWaitExpired,
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
	UE_LOG(LogUMoveset, Log, TEXT("%s UMovesetComponent received input token %s"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>("EInputToken", Token));
	if (CurrentState == nullptr)
	{
		GotoDefaultState();
		return ReceiveInputToken(Token);
	}

	for (int i = 0; i < CurrentState->PossibleTransitions.Num(); i++)
	{
		AMoveState *PossibleMove = CurrentState->PossibleTransitions[i].GetDefaultObject();
		if (PossibleMove->AllowedInputs.FilterInputToken(Token))
		{
			if (MyFighterState != nullptr && PossibleMove->StanceFilter.FilterStance(MyFighterState->GetStance()))
			{
				if (MyFighterState->UseSpecial(PossibleMove->SpecialCost))
				{
					// Found a state that we can enter.
					GotoState(CurrentState->PossibleTransitions[i]);
					return;
				}
			}
		}
	}
	// We weren't able to use any of the possible transitions or there were none to begin with.

	if (CurrentState->GetClass() != DefaultState.Get())
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

void UMovesetComponent::BeginSolo()
{
	//TODO
}

void UMovesetComponent::OnSoloCorrect(EInputToken Token)
{
	//TODO
}

void UMovesetComponent::OnSoloIncorrect()
{
	//TODO
}