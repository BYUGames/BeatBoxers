// copyright 2017 BYU Animation

#include "InputParserComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UInputParserComponent::UInputParserComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InputBufferLength = 0.2f;
	ComplexInputWindow = 0.2f;

	CurrentState = TSharedPtr<FInputParserState>(new FDefaultState(this));
}


// Called when the game starts
void UInputParserComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInputParserComponent::OnInputBufferTimer()
{
	InputBuffer = EInputToken::IE_None;
}

void UInputParserComponent::SetInputBuffer(EInputToken NewToken)
{
	InputBuffer = NewToken;
	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_InputBuffer,
		this,
		&UInputParserComponent::OnInputBufferTimer,
		InputBufferLength,
		false
	);
}

void UInputParserComponent::StartComboTimer()
{
	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_Combo,
		this,
		&UInputParserComponent::OnComboTimer,
		ComplexInputWindow,
		false
	);
}

void UInputParserComponent::OnComboTimer()
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(this)));
	}
}

void UInputParserComponent::PushInputToken(EInputToken NewToken)
{
	if (MyFighterState != nullptr)
	{
		if (MyFighterState->IsInputBlocked())
		{
			SetInputBuffer(NewToken);
		}
		else if (MyMoveset != nullptr)
		{
			MyMoveset->ReceiveInputToken(NewToken);
		}
	}
}


// Called every frame
void UInputParserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInputParserComponent::RegisterFighterState(TWeakObjectPtr<UObject> FighterState)
{
	if (!FighterState.IsValid())
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("UInputParserComponent %s given invalid object to register as FighterState."), *GetNameSafe(this));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(FighterState.Get());
		if (MyFighterState == nullptr)
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("UInputParserComponent %s given %s to register as FighterState, but it doesn't implement IFighterState."), *GetNameSafe(this), *GetNameSafe(FighterState.Get()));
		}
	}
}

void UInputParserComponent::RegisterMoveset(TWeakObjectPtr<UObject> Moveset)
{
	if (!Moveset.IsValid())
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("UInputParserComponent %s given invalid object to register as Moveset."), *GetNameSafe(this));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset.Get());
		if (MyMoveset == nullptr)
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("UInputParserComponent %s given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(this), *GetNameSafe(Moveset.Get()));
		}
	}
}

void UInputParserComponent::OnControlReturned()
{
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_InputBuffer))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_InputBuffer);
		if (MyMoveset != nullptr && InputBuffer != EInputToken::IE_None)
		{
			MyMoveset->ReceiveInputToken(InputBuffer);
			InputBuffer = EInputToken::IE_None;
		}
	}
}

void UInputParserComponent::InputAxisHorizontal(float Amount)
{
	if (MyFighterState != nullptr)
	{
		MyFighterState->SetMoveDirection(Amount);
	}
}

void UInputParserComponent::InputAxisVertical(float Amount)
{
	if (MyFighterState != nullptr)
	{
		if (Amount < 0)
		{
			MyFighterState->SetWantsToCrouch(true);
		}
		else
		{
			MyFighterState->SetWantsToCrouch(false);
			if (Amount > 0)
			{
				MyFighterState->Jump();
			}
		}
	}
}

void UInputParserComponent::InputActionLeft(bool IsUp)
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->InputActionLeft();
	}
}

void UInputParserComponent::InputActionRight(bool IsUp)
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->InputActionRight();
	}
}

void UInputParserComponent::InputActionDown(bool IsUp)
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->InputActionDown();
	}
}

void UInputParserComponent::InputActionUp(bool IsUp)
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->InputActionUp();
	}
}

void UInputParserComponent::InputActionPunch(bool IsUp)
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->InputActionPunch();
	}
}

void UInputParserComponent::InputActionKick(bool IsUp)
{
	if (CurrentState.IsValid())
	{
		CurrentState.Get()->InputActionKick();
	}
}

UInputParserComponent::FInputParserState::FInputParserState(UInputParserComponent *Parent)
{
	UInputParserComponent::FInputParserState::Parent = Parent;
}

void UInputParserComponent::FInputParserState::ChangeState(TSharedPtr<FInputParserState> NewState)
{
	if (Parent != nullptr)
	{
		TSharedPtr<FInputParserState> temp = Parent->CurrentState;
		Parent->CurrentState = NewState;
		if (IsComplex())
		{
			Parent->StartComboTimer();
		}
		if (temp.IsValid())
		{
			temp.Reset();
		}
	}
}

bool UInputParserComponent::FInputParserState::IsComplex() { return false; }

void UInputParserComponent::FInputParserState::InputActionLeft() { UE_LOG(LogBeatBoxers, Log, TEXT("FInputParserState::InputActionLeft()")); }
void UInputParserComponent::FInputParserState::InputActionRight() { UE_LOG(LogBeatBoxers, Log, TEXT("FInputParserState::InputActionRight()")); }
void UInputParserComponent::FInputParserState::InputActionDown() { UE_LOG(LogBeatBoxers, Log, TEXT("FInputParserState::InputActionDown()")); }
void UInputParserComponent::FInputParserState::InputActionUp() { UE_LOG(LogBeatBoxers, Log, TEXT("FInputParserState::InputActionUp()")); }
void UInputParserComponent::FInputParserState::InputActionPunch() { UE_LOG(LogBeatBoxers, Log, TEXT("FInputParserState::InputActionPunch()")); }
void UInputParserComponent::FInputParserState::InputActionKick() { UE_LOG(LogBeatBoxers, Log, TEXT("FInputParserState::InputActionKick()")); }

void UInputParserComponent::FDefaultState::InputActionLeft()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FDefaultState::InputActionLeft()"));
	if (Parent != nullptr)
	{
		ChangeState(TSharedPtr<FInputParserState>(new FPreLeftDashState(Parent)));
	}
}

void UInputParserComponent::FDefaultState::InputActionRight()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FDefaultState::InputActionRight()"));
	if (Parent != nullptr)
	{
		ChangeState(TSharedPtr<FInputParserState>(new FPreRightDashState(Parent)));
	}
}

void UInputParserComponent::FDefaultState::InputActionPunch()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FDefaultState::InputActionPunch()"));
	if (Parent != nullptr)
	{
		Parent->PushInputToken(EInputToken::IE_Punch);
	}
}

void UInputParserComponent::FDefaultState::InputActionKick()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FDefaultState::InputActionKick()"));
	if (Parent != nullptr)
	{
		Parent->PushInputToken(EInputToken::IE_Kick);
	}
}

bool UInputParserComponent::FPreLeftDashState::IsComplex() { return true; }

void UInputParserComponent::FPreLeftDashState::InputActionLeft()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FPreLeftDashState::InputActionLeft()"));
	if (Parent != nullptr)
	{
		Parent->PushInputToken(EInputToken::IE_DashLeft);
		ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(Parent)));
	}
}

void UInputParserComponent::FPreLeftDashState::InputActionPunch()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FPreLeftDashState::InputActionPunch()"));
	if (Parent != nullptr)
	{
		FDefaultState::InputActionPunch();
		ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(Parent)));
	}
}

void UInputParserComponent::FPreLeftDashState::InputActionKick()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FPreLeftDashState::InputActionKick()"));
	if (Parent != nullptr)
	{
		FDefaultState::InputActionKick();
		ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(Parent)));
	}
}

bool UInputParserComponent::FPreRightDashState::IsComplex() { return true; }

void UInputParserComponent::FPreRightDashState::InputActionRight()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FPreRightDashState::InputActionRight()"));
	if (Parent != nullptr)
	{
		Parent->PushInputToken(EInputToken::IE_DashRight);
		ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(Parent)));
	}
}

void UInputParserComponent::FPreRightDashState::InputActionPunch()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FPreRightDashState::InputActionPunch()"));
	if (Parent != nullptr)
	{
		FDefaultState::InputActionPunch();
		ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(Parent)));
	}
}

void UInputParserComponent::FPreRightDashState::InputActionKick()
{
	UE_LOG(LogBeatBoxers, Log, TEXT("FPreRightDashState::InputActionKick()"));
	if (Parent != nullptr)
	{
		FDefaultState::InputActionKick();
		ChangeState(TSharedPtr<FInputParserState>(new FDefaultState(Parent)));
	}
}