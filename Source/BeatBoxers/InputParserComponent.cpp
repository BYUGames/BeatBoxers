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
	LastDashInput = 0;
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
	if (MyMoveset != nullptr)
	{
		if (LastDashInput < 0)
		{
			LastDashInput = 0;
			PushInputToken(EInputToken::IE_DashLeft);
		}
		else
		{
			LastDashInput = -1;
			StartComboTimer();
		}
	}
}

void UInputParserComponent::InputActionRight(bool IsUp)
{
	if (MyMoveset != nullptr)
	{
		if (LastDashInput > 0)
		{
			LastDashInput = 0;
			PushInputToken(EInputToken::IE_DashRight);
		}
		else
		{
			LastDashInput = 1;
			StartComboTimer();
		}
	}
}

void UInputParserComponent::InputActionDown(bool IsUp)
{
	//nop
}

void UInputParserComponent::InputActionUp(bool IsUp)
{
	//nop
}

void UInputParserComponent::InputActionPunch(bool IsUp)
{
	PushInputToken(EInputToken::IE_Punch);
}

void UInputParserComponent::InputActionKick(bool IsUp)
{
	PushInputToken(EInputToken::IE_Kick);
}