// copyright 2017 BYU Animation

#include "InputParserComponent.h"
#include "GameFramework/Actor.h"
#include "FighterCharacter.h"

// Sets default values for this component's properties
UInputParserComponent::UInputParserComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InputBufferLength = 0.5f;
	ComplexInputWindow = 0.5f;

	CurrentStateClass = UInputParserDefaultState::StaticClass();
}


// Called when the game starts
void UInputParserComponent::BeginPlay()
{
	Super::BeginPlay();

	AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
	if (Fighter != nullptr)
	{
		InputBufferLength = Fighter->InputBufferLength;
		ComplexInputWindow = Fighter->ComplexInputWindow;
	}
}

void UInputParserComponent::OnInputBufferTimer()
{
	UE_LOG(LogUInputParser, VeryVerbose, TEXT("%s UInputParserComponent input buffer expired."), *GetNameSafe(GetOwner()));
	InputBuffer.token = EInputToken::IE_None;
}

void UInputParserComponent::SetInputBuffer(FBufferInputToken NewToken)
{
	UE_LOG(LogUInputParser, VeryVerbose, TEXT("%s UInputParserComponent setting input buffer to %s with %f on timer."), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>(TEXT("EInputToken"), NewToken.token), InputBufferLength);
	if (InputBuffer.token == EInputToken::IE_None)
		InputBuffer = NewToken;
	else
		InputBuffer.token = NewToken.token;
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
	UE_LOG(LogUInputParser, Verbose, TEXT("%s UInputParserComponent setting combo timer to %f."), *GetNameSafe(GetOwner()), ComplexInputWindow);
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
	UE_LOG(LogUInputParser, Verbose, TEXT("%s UInputParserComponent combo timer expired."), *GetNameSafe(GetOwner()));
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->ChangeState(this, UInputParserDefaultState::StaticClass());
	}
}

void UInputParserComponent::PushInputToken(EInputToken NewToken)
{
	FBufferInputToken bToken;
	bToken.token = NewToken;	
	bToken.accuracy = calcAccuracy();
	UE_LOG(LogUInputParser, Verbose, TEXT("%s UInputParserComponent Pushing input token %s"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>(TEXT("EInputToken"), NewToken));
	if (MyFighterState != nullptr)
	{
		if (MyFighterState->IsInputBlocked())
		{
			SetInputBuffer(bToken);
		}
		else if (MyMoveset != nullptr)
		{
			MyMoveset->ReceiveInputToken(bToken);
		}
	}
}


float UInputParserComponent::calcAccuracy()
{
	if (MyMusicBox != nullptr)
	{
		return MyMusicBox->GetBeatAccuracy();
	}
	return -1;
}

// Called every frame
void UInputParserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInputParserComponent::RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld)
{
	if (!FighterWorld.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UInputParserComponent given invalid object to register as FighterWorld."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterWorld = Cast<IFighterWorld>(FighterWorld.Get());
		if (MyFighterWorld == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UInputParserComponent given %s to register as FighterWorld, but it doesn't implement IFighterWorld."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterWorld.Get()));
		}
	}
}

void UInputParserComponent::RegisterFighter(TWeakObjectPtr<UObject> Fighter)
{
	if (!Fighter.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s UInputParserComponent given invalid object to register as Fighter."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighter = Cast<IFighter>(Fighter.Get());
		if (MyFighter == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s UInputParserComponent given %s to register as Fighter, but it doesn't implement IFighter."), *GetNameSafe(GetOwner()), *GetNameSafe(Fighter.Get()));
		}
	}
}

void UInputParserComponent::RegisterFighterState(TWeakObjectPtr<UObject> FighterState)
{
	if (!FighterState.IsValid())
	{
		UE_LOG(LogUInputParser, Error, TEXT("%s UInputParserComponent given invalid object to register as FighterState."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(FighterState.Get());
		if (MyFighterState == nullptr)
		{
			UE_LOG(LogUInputParser, Error, TEXT("%s UInputParserComponent given %s to register as FighterState, but it doesn't implement IFighterState."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterState.Get()));
		}
	}
}

void UInputParserComponent::RegisterMoveset(TWeakObjectPtr<UObject> Moveset)
{
	if (!Moveset.IsValid())
	{
		UE_LOG(LogUInputParser, Error, TEXT("%s UInputParserComponent given invalid object to register as Moveset."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset.Get());
		if (MyMoveset == nullptr)
		{
			UE_LOG(LogUInputParser, Error, TEXT("%s UInputParserComponent given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(GetOwner()), *GetNameSafe(Moveset.Get()));
		}
	}
}

void UInputParserComponent::RegisterMusicBox(TWeakObjectPtr<UObject> MusicBox)
{
	if (!MusicBox.IsValid())
	{
		UE_LOG(LogUMoveset, Error, TEXT("%s UInputParserComponent given invalid object to register as MusicBox."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyMusicBox = Cast<IMusicBox>(MusicBox.Get());
		if (MyMusicBox == nullptr)
		{
			UE_LOG(LogUMoveset, Error, TEXT("%s UInputParserComponent given %s to register as MusicBox, but it doesn't implement IMusicBox."), *GetNameSafe(GetOwner()), *GetNameSafe(MusicBox.Get()));
		}
	}
}


void UInputParserComponent::OnControlReturned()
{
	UE_LOG(LogUInputParser, Verbose, TEXT("%s UInputParserComponent OnControlReturned, input buffer = %s."), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>(TEXT("EInputToken"), InputBuffer.token));
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(TimerHandle_InputBuffer))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_InputBuffer);
		if (MyMoveset != nullptr && InputBuffer.token != EInputToken::IE_None)
		{
			// Preventing infinite loops.
			FBufferInputToken BufferToken = InputBuffer;
			InputBuffer.token = EInputToken::IE_None;
			MyMoveset->ReceiveInputToken(BufferToken);
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
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionLeft(this);
	}
}

void UInputParserComponent::InputActionRight(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionRight(this);
	}
}

void UInputParserComponent::InputActionDown(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionDown(this);
	}
}

void UInputParserComponent::InputActionUp(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionUp(this);
	}
}

void UInputParserComponent::InputActionLight(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionLight(this);
	}
}

void UInputParserComponent::InputActionMedium(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionMedium(this);
	}
}

void UInputParserComponent::InputActionHeavy(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionHeavy(this);
	}
}

void UInputParserState::ChangeState(UInputParserComponent *Parser, TSubclassOf<UInputParserState> NewState)
{
	if (Parser != nullptr)
	{
		if (NewState.Get() == nullptr)
		{
			UE_LOG(LogUInputParser, Error, TEXT("%s UInputParserComponent FInputParserState requested a change to an invalid state."), *GetNameSafe(Parser->GetOwner()));
			Parser->CurrentStateClass = UInputParserDefaultState::StaticClass();
		}
		else
		{
			Parser->CurrentStateClass = NewState;
			UE_LOG(LogUInputParser, Verbose, TEXT("FInputParserState::ChangeState current state IsComplex() is %s."), (IsComplex()) ? TEXT("TRUE") : TEXT("FALSE"));
			if (NewState.GetDefaultObject()->IsComplex())
			{
				Parser->StartComboTimer();
			}
		}
	}
	else
	{
		UE_LOG(LogUInputParser, Error, TEXT("An UInputParserState was requested to change state without a pointer to a valid UInputParserComponent."));
	}
}

bool UInputParserState::IsComplex() { return false; }

void UInputParserState::InputActionLeft(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionLeft()")); }
void UInputParserState::InputActionRight(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionRight()")); }
void UInputParserState::InputActionDown(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionDown()")); }
void UInputParserState::InputActionUp(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionUp()")); }
void UInputParserState::InputActionLight(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionLight()")); }
void UInputParserState::InputActionMedium(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionMedium()")); }
void UInputParserState::InputActionHeavy(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionHeavy()")); }

void UInputParserDefaultState::InputActionLeft(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLeft()"));
	if (Parser != nullptr)
	{
		ChangeState(Parser, UPreLeftDashState::StaticClass());
	}
}

void UInputParserDefaultState::InputActionRight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionRight()"));
	if (Parser != nullptr)
	{
		ChangeState(Parser, UPreRightDashState::StaticClass());
	}
}

void UInputParserDefaultState::InputActionLight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLight()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Light);
	}
}

void UInputParserDefaultState::InputActionMedium(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionMedium()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Medium);
	}
}

void UInputParserDefaultState::InputActionHeavy(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionHeavy()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Heavy);
	}
}

bool UPreLeftDashState::IsComplex() { return true; }

void UPreLeftDashState::InputActionLeft(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionLeft()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_DashLeft);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionLight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionLight()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionLight(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionMedium(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionMedium()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionMedium(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionHeavy(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionHeavy()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionHeavy(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

bool UPreRightDashState::IsComplex() { return true; }

void UPreRightDashState::InputActionRight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionRight()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_DashRight);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionLight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionLight()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionLight(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionMedium(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionMedium()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionMedium(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionHeavy(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionHeavy()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionHeavy(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}