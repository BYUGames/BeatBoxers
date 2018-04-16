// copyright 2017 BYU Animation

#include "InputParserComponent.h"
#include "GameFramework/Actor.h"
#include "../BBGameInstance.h"
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
	if (MyMusicBox != nullptr)
	{
		UE_LOG(LogBeatTiming, Verbose, TEXT("%s UInputParserComponent Pushing input token with accuracy of %f. Time to next beat %f / %f.")
			, *GetNameSafe(GetOwner())
			, bToken.accuracy
			, MyMusicBox->GetTimeToNextBeat()
			, MyMusicBox->GetTimeBetweenBeats()
			);
	}
	UE_LOG(LogUInputParser, Verbose, TEXT("%s UInputParserComponent Pushing input token %s"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>(TEXT("EInputToken"), NewToken));
	if (MyFighterState != nullptr)
	{
		if (MyFighterState->IsInputBlocked() || MyFighterState->IsStunned() || MyFighter->HasAttackedThisBeat())
		{
			if (MyFighter->HasAttackedThisBeat()) {
				bToken.accuracy = 0.5f;
			}
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
	PushInputToken(InputBuffer.token);
}



void UInputParserComponent::InputAxisHorizontal(float Amount)
{

	bool input360axis = false;
	float defaultHorizontal = 0;
	float P1360axisUpValue = 0;
	float P1360axisUpRightValue = 0;
	float P1360axisRightValue = 0;
	float P1360axisBottomRightValue = 0;
	float P1360axisBottomValue = 0;
	float P1360axisBottomLeftValue = 0;
	float P1360axisLeftValue = 0;
	float P1360axisTopLeftValue = 0;

	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr) {
		input360axis = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360Axis;
		defaultHorizontal = GetWorld()->GetGameInstance<UBBGameInstance>()->P1HorizontalDefaultValue;
		P1360axisUpValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisUpValue;
		P1360axisUpRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisUpRightValue;
		P1360axisRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisRightValue;
		P1360axisBottomRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisBottomRightValue;
		P1360axisBottomValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisBottomValue;
		P1360axisBottomLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisBottomLeftValue;
		P1360axisLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisLeftValue;
		P1360axisTopLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisTopLeftValue;
	}
	float AdjustedAmount = defaultHorizontal;


	if (input360axis)
	{
		float closeAmount = Amount - P1360axisUpValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = 1;
		closeAmount = Amount - P1360axisUpRightValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = 1;
		closeAmount = Amount - P1360axisRightValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = 1;
		closeAmount = Amount - P1360axisBottomRightValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = 1;
		closeAmount = Amount - P1360axisBottomValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = 0;
		closeAmount = Amount - P1360axisBottomLeftValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = -1;
		closeAmount = Amount - P1360axisLeftValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = -1;
		closeAmount = Amount - P1360axisTopLeftValue;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = -1;
		closeAmount = Amount - defaultHorizontal;
		if (fabs(closeAmount) < .01f) 
			AdjustedAmount = 0;
	}
	else {
		if (Amount > (defaultHorizontal + .1)) 
			AdjustedAmount = 1;
		else if (Amount < (defaultHorizontal - .1)) 
			AdjustedAmount = -1;
		else 
			AdjustedAmount = 0;
	}

	if(AdjustedAmount > -1)
		HasDashedLeft = false;
	if (AdjustedAmount < 1)
		HasDashedRight = false;


	HorizontalMovement = AdjustedAmount;
	if (MyFighterState != nullptr)
	{
		MyFighterState->SetMoveDirection(AdjustedAmount);
		MyFighterState->SetHorizontalDirection(AdjustedAmount);
	}

	if (HoldingBlock && AdjustedAmount == -1 && !HasDashedLeft) {
		HasDashedLeft = true;
		if (GetFighterFacing() > 0)
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
	}

	if (HoldingBlock && AdjustedAmount == 1 && !HasDashedRight) {
		HasDashedRight = true;
		if (GetFighterFacing() > 0)
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
	}

}

void UInputParserComponent::InputAxisHorizontalP2(float Amount)
{
	
	bool P2input360axis = false;
	float P2defaultHorizontal = 0;
	float P2360axisUpValue = 0;
	float P2360axisUpRightValue = 0;
	float P2360axisRightValue = 0;
	float P2360axisBottomRightValue = 0;
	float P2360axisBottomValue = 0;
	float P2360axisBottomLeftValue = 0;
	float P2360axisLeftValue = 0;
	float P2360axisTopLeftValue = 0;

	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr) {
		P2input360axis = GetWorld()->GetGameInstance<UBBGameInstance>()->P21360Axis;
		P2defaultHorizontal = GetWorld()->GetGameInstance<UBBGameInstance>()->P2HorizontalDefaultValue;
		P2360axisUpValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisUpValue;
		P2360axisUpRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisUpRightValue;
		P2360axisRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisRightValue;
		P2360axisBottomRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomRightValue;
		P2360axisBottomValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomValue;
		P2360axisBottomLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomLeftValue;
		P2360axisLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisLeftValue;
		P2360axisTopLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisTopLeftValue;
	}
	float AdjustedAmount = P2defaultHorizontal;


	if (P2input360axis)
	{
		float closeAmount = Amount - P2360axisUpValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - P2360axisUpRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - P2360axisRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - P2360axisBottomRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - P2360axisBottomValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
		closeAmount = Amount - P2360axisBottomLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - P2360axisLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - P2360axisTopLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - P2defaultHorizontal;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
	}
	else {
		if (Amount > (P2defaultHorizontal + .1))
			AdjustedAmount = 1;
		else if (Amount < (P2defaultHorizontal - .1))
			AdjustedAmount = -1;
		else
			AdjustedAmount = 0;
	}

	if (AdjustedAmount > -1)
		HasDashedLeft = false;
	if (AdjustedAmount < 1)
		HasDashedRight = false;


	HorizontalMovement = AdjustedAmount;
	if (MyFighterState != nullptr)
	{
		MyFighterState->SetMoveDirection(AdjustedAmount);
		MyFighterState->SetHorizontalDirection(AdjustedAmount);
	}

	if (HoldingBlock && AdjustedAmount == -1 && !HasDashedLeft) {
		HasDashedLeft = true;
		if (GetFighterFacing() > 0)
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
	}

	if (HoldingBlock && AdjustedAmount == 1 && !HasDashedRight) {
		HasDashedRight = true;
		if (GetFighterFacing() > 0)
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
	}
}

void UInputParserComponent::InputAxisVertical(float Amount)
{
	if (MyFighterState != nullptr && !MyFighterState->IsInCrouchMove())
	{
		MyFighterState->SetVerticalDirection(Amount);
		if (Amount < -.25f)
		{
			
			MyFighterState->SetWantsToCrouch(true);
		}
		else
		{
			MyFighterState->SetWantsToCrouch(false);
			if (Amount > 0.5f)
			{
				MyFighterState->Jump();
			}
		}
	}
	else if(MyFighter != nullptr)
	{
		MyFighter->SetWantsToCrouch(true);
	}
}

void UInputParserComponent::InputActionDodge(bool IsUp)
{
	if (MyFighterState != nullptr)
	{
		if (HorizontalMovement == 0 || FMath::Sign(GetFighterFacing()) ==  FMath::Sign(HorizontalMovement))
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
	}
}


void UInputParserComponent::InputActionBlock(bool IsUp)
{
	if (MyFighterState != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionBlock(this);
		MyFighterState->Block();
		HoldingBlock = true;
	}
}

void UInputParserComponent::InputActionStopBlock(bool IsUp)
{
	if (MyFighterState != nullptr)
	{
		MyFighterState->StopBlock();
		HoldingBlock = false;
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

float UInputParserComponent::GetOpponentDirection()
{
	if (MyFighter != nullptr)
	{
		return MyFighter->GetOpponentDirection();
	}
	return 0;
}

float UInputParserComponent::GetFighterFacing()
{
	if (MyFighter != nullptr)
	{
		return MyFighter->GetFacing();
	}
	return 0;
}

void UInputParserComponent::InputActionDashLeft(bool Isup)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		if (GetFighterFacing() > 0)
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
	}
}

void UInputParserComponent::InputActionDashRight(bool Isup)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		if (GetFighterFacing() > 0)
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashForward(this);
		}
		else
		{
			CurrentStateClass.GetDefaultObject()->InputActionDashBackwards(this);
		}
	}
}

void UInputParserComponent::InputActionLight(bool IsUp)
{
	UE_LOG(LogUInputParser, Error, TEXT("light"));
	if (CurrentStateClass.Get() != nullptr)
	{
		if (MyFighterState->GetCurrentVerticalDirection() >= 0) {
			if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
				CurrentStateClass.GetDefaultObject()->InputActionLightForward(this);
			}
			else if (FMath::Sign(GetFighterFacing()) == -FMath::Sign(HorizontalMovement)) {
				CurrentStateClass.GetDefaultObject()->InputActionLightBack(this);
			}
			else {
				CurrentStateClass.GetDefaultObject()->InputActionLight(this);
			}
		}
		else
			CurrentStateClass.GetDefaultObject()->InputActionLightCrouch(this);
	}
}

void UInputParserComponent::InputActionMedium(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		if (MyFighterState->GetCurrentVerticalDirection() >= 0) {
			CurrentStateClass.GetDefaultObject()->InputActionMedium(this);
		}
		else
			CurrentStateClass.GetDefaultObject()->InputActionMediumCrouch(this);
	}
}

void UInputParserComponent::InputActionHeavy(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		if (MyFighterState->GetCurrentVerticalDirection() >= 0)
			CurrentStateClass.GetDefaultObject()->InputActionHeavy(this);
		else
			CurrentStateClass.GetDefaultObject()->InputActionHeavyCrouch(this);
	}
}

void UInputParserComponent::InputActionSpecial1(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionSpecial1(this);
	}
}

void UInputParserComponent::InputActionSpecial2(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionSpecial2(this);
	}
}

void UInputParserComponent::InputActionSpecial3(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionSpecial3(this);
	}
}

void UInputParserComponent::InputActionSuper(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		CurrentStateClass.GetDefaultObject()->InputActionSuper(this);
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
void UInputParserState::InputActionLightCrouch(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionLightCrouch()")); }
void UInputParserState::InputActionLightForward(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionLightForward()")); }
void UInputParserState::InputActionLightBack(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionLightBack()")); }
void UInputParserState::InputActionMedium(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionMedium()")); }
void UInputParserState::InputActionMediumCrouch(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionMediumCrouch()")); }
void UInputParserState::InputActionHeavy(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionHeavy()")); }
void UInputParserState::InputActionHeavyCrouch(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionHeavyCrouch()")); }
void UInputParserState::InputActionSpecial1(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionSpecial1()")); }
void UInputParserState::InputActionSpecial2(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionSpecial2()")); }
void UInputParserState::InputActionSpecial3(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionSpecial3()")); }
void UInputParserState::InputActionSuper(UInputParserComponent *Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionSuper()")); }
void UInputParserState::InputActionBlock(UInputParserComponent * Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionBlock()")); }
void UInputParserState::InputActionDashForward(UInputParserComponent * Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionDashForward()")); }
void UInputParserState::InputActionDashBackwards(UInputParserComponent * Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionDashBackwards()")); }

void UInputParserDefaultState::InputActionDashBackwards(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionDashBackwards()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_DashBackward);
	}
}

void UInputParserDefaultState::InputActionDashForward(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionDashForward()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_DashForward);
	}
}


void UInputParserDefaultState::InputActionLeft(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLeft()"));
	if (Parser != nullptr)
	{
		//ChangeState(Parser, UPreLeftDashState::StaticClass());
	}
}


void UInputParserDefaultState::InputActionRight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionRight()"));
	if (Parser != nullptr)
	{
		//ChangeState(Parser, UPreRightDashState::StaticClass());
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

void UInputParserDefaultState::InputActionLightForward(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightForward()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightForward);
	}
}

void UInputParserDefaultState::InputActionLightBack(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightBack()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightBack);
	}
}

void UInputParserDefaultState::InputActionLightCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightCrouch()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightCrouch);
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

void UInputParserDefaultState::InputActionMediumCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionMedium()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_MediumCrouch);
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

void UInputParserDefaultState::InputActionHeavyCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionHeavy()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_HeavyCrouch);
	}
}

void UInputParserDefaultState::InputActionSpecial1(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionSpecial1()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Special1);
	}
}

void UInputParserDefaultState::InputActionSpecial2(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionSpecial2()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Special2);
	}
}

void UInputParserDefaultState::InputActionSpecial3(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionSpecial3()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Special3);
	}
}

void UInputParserDefaultState::InputActionSuper(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionSuper()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Super);
	}
}

void UInputParserDefaultState::InputActionBlock(UInputParserComponent * Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionBlock()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_Block);
	}
}



bool UPreLeftDashState::IsComplex() { return true; }

void UPreLeftDashState::InputActionLeft(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionLeft()"));
	if (Parser != nullptr)
	{
		if (Parser->GetFighterFacing() > 0)
		{
			Parser->PushInputToken(EInputToken::IE_DashBackward);
		}
		else
		{
			Parser->PushInputToken(EInputToken::IE_DashForward);
		}
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

void UPreLeftDashState::InputActionLightForward(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightForward()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightForward);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionLightBack(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightBack()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightBack);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionLightCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionLightCrouch()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionLightCrouch(Parser);
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

void UPreLeftDashState::InputActionMediumCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionMediumCrouch()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionMediumCrouch(Parser);
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

void UPreLeftDashState::InputActionHeavyCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionHeavy()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionHeavyCrouch(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionSpecial1(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionSpecial1()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionSpecial1(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionSpecial2(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionSpecial2()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionSpecial2(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionSpecial3(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionSpecial3()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionSpecial3(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

bool UPreRightDashState::IsComplex() { return true; }

void UPreRightDashState::InputActionRight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionRight()"));
	if (Parser != nullptr)
	{
		if (Parser->GetFighterFacing() > 0)
		{
			Parser->PushInputToken(EInputToken::IE_DashForward);
		}
		else
		{
			Parser->PushInputToken(EInputToken::IE_DashBackward);
		}
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

void UPreRightDashState::InputActionLightForward(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightForward()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightForward);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionLightBack(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionLightBack()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_LightBack);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionLightCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionLightCrouch()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionLightCrouch(Parser);
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

void UPreRightDashState::InputActionMediumCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionMediumCrouch()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionMediumCrouch(Parser);
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

void UPreRightDashState::InputActionHeavyCrouch(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionHeavyCrouch()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionHeavyCrouch(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionSpecial1(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionSpecial1()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionSpecial1(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionSpecial2(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionSpecial2()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionSpecial2(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionSpecial3(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionSpecial3()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionSpecial3(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}