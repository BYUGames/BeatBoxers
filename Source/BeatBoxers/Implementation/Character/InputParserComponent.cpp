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
	UE_LOG(LogUInputParser, Error, TEXT("%s UInputParserComponent input buffer expired."), *GetNameSafe(GetOwner()));
	InputBuffer.token = EInputToken::IE_None;
}

void UInputParserComponent::SetInputBuffer(FBufferInputToken NewToken)
{
	UE_LOG(LogUInputParser, VeryVerbose, TEXT("%s UInputParserComponent setting input buffer to %s with %f on timer."), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>(TEXT("EInputToken"), NewToken.token), InputBufferLength);
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
	UE_LOG(LogUInputParser, Verbose, TEXT("%s UInputParserComponent Pushing input token %s with accuracy %f"), *GetNameSafe(GetOwner()), *GetEnumValueToString<EInputToken>(TEXT("EInputToken"), NewToken), bToken.accuracy);
	if (MyFighterState != nullptr)
	{
		if ((HasInputtedThisBeat || (MyFighterState->IsMidMove() || (MyFighterState->previousBeatHadAttack && !(MyFighterWorld->IsOnBeat(bToken.accuracy)))))
			&& ((NewToken == EInputToken::IE_DashForward) || (NewToken == EInputToken::IE_DashBackward))) {
			//either you have already inputted this beat
			//you're in mid move
			//or the move just recently ended and you're not onbeat
			AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
			AFighterCharacter *Opponent = Cast<AFighterCharacter>(Fighter->MyOpponent.Get());
			UMovesetComponent *Moveset = Cast<UMovesetComponent>(Fighter->GetMoveset());
			if (!(Opponent->FighterState->IsStunned() && ((Moveset->CurrentState == Moveset->GrabbingState) || (Moveset->CurrentState == Moveset->GrabbingOffbeatState) || Opponent->FighterState->IsGrabbed()))
				&& (Moveset->CurrentState != Moveset->SuperState)
				//&& (Moveset->CurrentState != Moveset->ParryState) 
				&& (Moveset->CurrentState != Moveset->DashState)
				&& (Moveset->CurrentState != Moveset->DashBackState)
				) {
				if ((Moveset->CurrentState == Moveset->BlockState) || (Moveset->CurrentState == Moveset->DefaultState)) {//cancel parry into normal dodge
					Moveset->GotoDefaultState();
					//bToken.accuracy = 0.0f;
					Fighter->HasUsedMoveAndHasYetToLand = false;
					HasInputtedThisBeat = false;
					InputBuffer.token = EInputToken::IE_None;
					MyMoveset->ReceiveInputToken(bToken);
					return;
				}
				if (Fighter->FighterState->GetSpecial() >= 25) {//cancel all other valid moves into cancel
					if (NewToken == EInputToken::IE_DashForward)bToken.token = EInputToken::IE_DashCancelForward;
					if (NewToken == EInputToken::IE_DashBackward)bToken.token = EInputToken::IE_DashCancelBackward;
					bToken.accuracy = 0.0f;
					Fighter->HasUsedMoveAndHasYetToLand = false;
					HasInputtedThisBeat = false;
					InputBuffer.token = EInputToken::IE_None;
					MyMoveset->ReceiveInputToken(bToken);
					return;
				}
			}
		}

		if (HasInputtedThisBeat)
		{
			HasInputtedThisBeat = true;
			bToken.accuracy = 0.5f;
			SetInputBuffer(bToken);
		}
		else if (MyFighterState->IsInputBlocked() || MyFighterState->IsStunned())
		{
			HasInputtedThisBeat = true;
			SetInputBuffer(bToken);
		}
		else if (MyMoveset != nullptr)
		{
			HasInputtedThisBeat = true;
			InputBuffer.token = EInputToken::IE_None;
			MyMoveset->ReceiveInputToken(bToken);
		}
	}
}

void UInputParserComponent::PushInputTokenWithAccuracy(FBufferInputToken NewToken)
{
	FBufferInputToken bToken;
	bToken = NewToken;

	if (MyFighterState != nullptr)
	{
		if (MyFighterState->IsInputBlocked() || MyFighterState->IsStunned())
		{
		}
		else if (MyMoveset != nullptr)
		{
			InputBuffer.token = EInputToken::IE_None;
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
	if (!MyFighterState->IsInputBlocked() && !MyFighterState->IsStunned() && InputBuffer.token != EInputToken::IE_None) {
		PushInputTokenWithAccuracy(InputBuffer);
	}
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
	float AdjustedAmount = 0;


	if (input360axis)
	{
		float closeAmount = Amount - P1360axisUpValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
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
		AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
		//if (!Fighter->IsToRightOfOpponent())
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
		AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
		//if (!Fighter->IsToRightOfOpponent())
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
	bool input360axis = false;
	float defaultHorizontal = 0;
	float player360axisUpValue = 0;
	float player360axisUpRightValue = 0;
	float player360axisRightValue = 0;
	float player360axisBottomRightValue = 0;
	float player360axisBottomValue = 0;
	float player360axisBottomLeftValue = 0;
	float player360axisLeftValue = 0;
	float player360axisTopLeftValue = 0;

	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr) {
		input360axis = GetWorld()->GetGameInstance<UBBGameInstance>()->P21360Axis;
		defaultHorizontal = GetWorld()->GetGameInstance<UBBGameInstance>()->P2HorizontalDefaultValue;
		player360axisUpValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisUpValue;
		player360axisUpRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisUpRightValue;
		player360axisRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisRightValue;
		player360axisBottomRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomRightValue;
		player360axisBottomValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomValue;
		player360axisBottomLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomLeftValue;
		player360axisLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisLeftValue;
		player360axisTopLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisTopLeftValue;
	}
	float AdjustedAmount = defaultHorizontal;


	if (input360axis)
	{
		float closeAmount = Amount - player360axisUpValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
		closeAmount = Amount - player360axisUpRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - player360axisRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - player360axisBottomRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - player360axisBottomValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
		closeAmount = Amount - player360axisBottomLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - player360axisLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - player360axisTopLeftValue;
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
		AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
	//	if (!Fighter->IsToRightOfOpponent())
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
		AFighterCharacter *Fighter = Cast<AFighterCharacter>(GetOwner());
//		if (!Fighter->IsToRightOfOpponent())
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
	bool input360axis = false;
	float defaultVertical = 0;
	float player360axisUpValue = 0;
	float player360axisUpRightValue = 0;
	float player360axisRightValue = 0;
	float player360axisBottomRightValue = 0;
	float player360axisBottomValue = 0;
	float player360axisBottomLeftValue = 0;
	float player360axisLeftValue = 0;
	float player360axisTopLeftValue = 0;

	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr) {
		input360axis = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360Axis;
		defaultVertical = GetWorld()->GetGameInstance<UBBGameInstance>()->P1VerticalDefaultValue;
		player360axisUpValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisUpValue;
		player360axisUpRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisUpRightValue;
		player360axisRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisRightValue;
		player360axisBottomRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisBottomRightValue;
		player360axisBottomValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisBottomValue;
		player360axisBottomLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisBottomLeftValue;
		player360axisLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisLeftValue;
		player360axisTopLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P1360axisTopLeftValue;
	}
	float AdjustedAmount = 0;


	if (input360axis)
	{
		float closeAmount = Amount - player360axisUpValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - player360axisUpRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - player360axisRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
		closeAmount = Amount - player360axisBottomRightValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - player360axisBottomValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - player360axisBottomLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = -1;
		closeAmount = Amount - player360axisLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
		closeAmount = Amount - player360axisTopLeftValue;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 1;
		closeAmount = Amount - defaultVertical;
		if (fabs(closeAmount) < .01f)
			AdjustedAmount = 0;
	}
	else {
		if (Amount > (defaultVertical + .1))
			AdjustedAmount = 1;
		else if (Amount < (defaultVertical - .1))
			AdjustedAmount = -1;
		else
			AdjustedAmount = 0;
	}

	if (AdjustedAmount == -1) {
		if (FMath::Sign(GetFighterFacing()) == FMath::Sign(-HorizontalMovement)) {
			//downleft
			ParseCurrentHeldDirection(1);
		}
		else if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
			//downright
			ParseCurrentHeldDirection(3);
		}
		else {
			//down
			ParseCurrentHeldDirection(2);
		}
	}
	else if (AdjustedAmount == 1) {
		if (FMath::Sign(GetFighterFacing()) == FMath::Sign(-HorizontalMovement)) {
			//upleft
			ParseCurrentHeldDirection(7);
		}
		else if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
			//upright
			ParseCurrentHeldDirection(9);
		}
		else {
			//up
			ParseCurrentHeldDirection(8);
		}
	}
	else {
		if (FMath::Sign(GetFighterFacing()) == FMath::Sign(-HorizontalMovement)) {
			//left
			ParseCurrentHeldDirection(4);
		}
		else if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
			//right
			ParseCurrentHeldDirection(6);
		}
		else {
			//neutral
			ParseCurrentHeldDirection(5);
		}
	}


	if (MyFighterState != nullptr && !MyFighterState->IsInCrouchMove())
	{
		MyFighterState->SetVerticalDirection(AdjustedAmount);
		if (AdjustedAmount < 0) {
			MyFighterState->SetWantsToCrouch(true);

		}
		else {
			MyFighterState->SetWantsToCrouch(false);
			if (AdjustedAmount > 0)
			{
				//MyFighterState->Jump();//
			}
		}
	}
	else if (MyFighter != nullptr)
	{
		MyFighter->SetWantsToCrouch(true);
	}
}
void UInputParserComponent::ParseCurrentHeldDirection(int NumpadDirection) {
	if (NumpadDirection != 1 && NumpadDirection != 2 && NumpadDirection != 3) {
		Cast<AFighterCharacter>(MyFighter)->UnCrouch();
	}
	else {
		Cast<AFighterCharacter>(MyFighter)->Crouch();
	}

	if (NumpadDirection != 7 && NumpadDirection != 8 && NumpadDirection != 9) {
		CurrentHeldDirection = NumpadDirection;
	}
	if (PreviousDirections.size() > 0) {
		if ((PreviousDirections.top() == NumpadDirection) && (NumpadDirection != 7) && (NumpadDirection != 8) && (NumpadDirection != 9)) {
			return;

		}
	}
	if (NumpadDirection == 5) {
		return;
	}
	PreviousDirections.push(NumpadDirection);
	if (CurrentHeldDirection != 7 && CurrentHeldDirection != 8 && CurrentHeldDirection != 9) {
		if (NumpadDirection == 7 || NumpadDirection == 8 || NumpadDirection == 9) {
			if (NumpadDirection == 7) {
				MyFighterState->Jump(-1);
			}
			else if (NumpadDirection == 8) {
				MyFighterState->Jump(0);
			}
			else if (NumpadDirection == 9) {
				MyFighterState->Jump(1);
			}
		}

	}
	if (CurrentHeldDirection != NumpadDirection) {
		CurrentHeldDirection = NumpadDirection;
	}

	GetOwner()->GetWorldTimerManager().SetTimer(
		TimerHandle_MotionTimer,
		this,
		&UInputParserComponent::OnMotionTimer,
		.2,
		false
	);

	//compare with previous direction held
	//if different, replace previous direction held
	//and add to previous 8 moves
	//timer so this has to be done relatively close to the time you did the motion
}

void UInputParserComponent::OnMotionTimer()
{
	PreviousDirections.push(10);
	CurrentHeldDirection = 10;
}

void UInputParserComponent::InputAxisVerticalP2(float Amount)
{
	bool input360axis = false;
	float defaultVertical = 0;
	float player360axisUpValue = 0;
	float player360axisUpRightValue = 0;
	float player360axisRightValue = 0;
	float player360axisBottomRightValue = 0;
	float player360axisBottomValue = 0;
	float player360axisBottomLeftValue = 0;
	float player360axisLeftValue = 0;
	float player360axisTopLeftValue = 0;

	if (GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr) {
		input360axis = GetWorld()->GetGameInstance<UBBGameInstance>()->P21360Axis;
		defaultVertical = GetWorld()->GetGameInstance<UBBGameInstance>()->P2VerticalDefaultValue;
		player360axisUpValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisUpValue;
		player360axisUpRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisUpRightValue;
		player360axisRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisRightValue;
		player360axisBottomRightValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomRightValue;
		player360axisBottomValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomValue;
		player360axisBottomLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisBottomLeftValue;
		player360axisLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisLeftValue;
		player360axisTopLeftValue = GetWorld()->GetGameInstance<UBBGameInstance>()->P2360axisTopLeftValue;
	}
	float AdjustedAmount = 0;


	if (input360axis)
	{
		float closeAmount = Amount - player360axisUpValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = 1;
		}
		closeAmount = Amount - player360axisUpRightValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = 1;
		}
		closeAmount = Amount - player360axisRightValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = 0;
		}
		closeAmount = Amount - player360axisBottomRightValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = -1;
		}
		closeAmount = Amount - player360axisBottomValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = -1;
		}
		closeAmount = Amount - player360axisBottomLeftValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = -1;
		}
		closeAmount = Amount - player360axisLeftValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = 0;
		}
		closeAmount = Amount - player360axisTopLeftValue;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = 1;
		}
		closeAmount = Amount - defaultVertical;
		if (fabs(closeAmount) < .01f) {
			AdjustedAmount = 0;
		}
	}
	else {
		if (Amount > (defaultVertical + .1))
			AdjustedAmount = 1;
		else if (Amount < (defaultVertical - .1))
			AdjustedAmount = -1;
		else
			AdjustedAmount = 0;
	}

	if (AdjustedAmount == -1) {
		if (FMath::Sign(GetFighterFacing()) == FMath::Sign(-HorizontalMovement)) {
			//downleft
			ParseCurrentHeldDirection(1);
		}
		else if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
			//downright
			ParseCurrentHeldDirection(3);
		}
		else {
			//down
			ParseCurrentHeldDirection(2);
		}
	}
	else if (AdjustedAmount == 1) {
		if (FMath::Sign(GetFighterFacing()) == FMath::Sign(-HorizontalMovement)) {
			//upleft
			ParseCurrentHeldDirection(7);
		}
		else if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
			//upright
			ParseCurrentHeldDirection(9);
		}
		else {
			//up
			ParseCurrentHeldDirection(8);
		}
	}
	else {
		if (FMath::Sign(GetFighterFacing()) == FMath::Sign(-HorizontalMovement)) {
			//left
			ParseCurrentHeldDirection(4);
		}
		else if (FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement)) {
			//right
			ParseCurrentHeldDirection(6);
		}
		else {
			//neutral
			ParseCurrentHeldDirection(5);
		}
	}


	if (MyFighterState != nullptr && !MyFighterState->IsInCrouchMove())
	{
		MyFighterState->SetVerticalDirection(AdjustedAmount);
		if (AdjustedAmount < 0) {
			MyFighterState->SetWantsToCrouch(true);
		}
		else {
			MyFighterState->SetWantsToCrouch(false);
			if (AdjustedAmount > 0)
			{
				//	MyFighterState->Jump();
			}
		}
	}
	else if (MyFighter != nullptr)
	{
		MyFighter->SetWantsToCrouch(true);
	}
}


void UInputParserComponent::InputActionDodge(bool IsUp)
{
	if (MyFighterState != nullptr)
	{
		if (HorizontalMovement == 0 || FMath::Sign(GetFighterFacing()) == FMath::Sign(HorizontalMovement))
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

void UInputParserComponent::InputActionQCF(bool IsUp)
{
}

void UInputParserComponent::InputActionQCB(bool IsUp)
{
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
		if (!Cast<AFighterCharacter>(GetOwner())->IsToRightOfOpponent())
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
		//if (!Cast<AFighterCharacter>(GetOwner())->IsToRightOfOpponent())
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
	if (CurrentStateClass.Get() != nullptr)
	{
		//if (!HoldingBlock) {
		if (PreviousDirections.size() > 1) {
			if ((PreviousDirections.top() == 3)) {
				PreviousDirections.pop();
				if ((PreviousDirections.top() == 2) || (PreviousDirections.top() == 6)) {
					PreviousDirections.empty();
					//quarter circle forward
					CurrentStateClass.GetDefaultObject()->InputActionQCF(this);
					return;
				}
			}
			else if ((PreviousDirections.top() == 6)) {
				PreviousDirections.pop();
				if ((PreviousDirections.top() == 2) || (PreviousDirections.top() == 3)) {
					PreviousDirections.empty();
					//quarter circle forward
					CurrentStateClass.GetDefaultObject()->InputActionQCF(this);
					return;
				}
			}


			else if ((PreviousDirections.top() == 4)) {
				PreviousDirections.pop();
				if ((PreviousDirections.top() == 1) || (PreviousDirections.top() == 2)) {
					PreviousDirections.empty();
					//quarter circle back
					CurrentStateClass.GetDefaultObject()->InputActionQCB(this);
					return;
				}
			}
			else if ((PreviousDirections.top() == 1)) {
				PreviousDirections.pop();
				if ((PreviousDirections.top() == 2) || (PreviousDirections.top() == 4)) {
					PreviousDirections.empty();
					//quarter circle back
					CurrentStateClass.GetDefaultObject()->InputActionQCB(this);
					return;
				}
			}
			else if ((PreviousDirections.top() == 2)) {
				PreviousDirections.pop();
				if ((PreviousDirections.top() == 1) || (PreviousDirections.top() == 4)) {
					PreviousDirections.empty();
					//quarter circle back
					CurrentStateClass.GetDefaultObject()->InputActionQCB(this);
					return;
				}
				else if ((PreviousDirections.top() == 3) || (PreviousDirections.top() == 6)) {
					PreviousDirections.empty();
					//quarter circle forward
					CurrentStateClass.GetDefaultObject()->InputActionQCF(this);
					return;
				}
			}
		}


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
		//}
	}
}

void UInputParserComponent::InputActionMedium(bool IsUp)
{
	if (CurrentStateClass.Get() != nullptr)
	{
		//if (!HoldingBlock) {
		if (MyFighterState->GetCurrentVerticalDirection() >= 0) {
			CurrentStateClass.GetDefaultObject()->InputActionMedium(this);
		}
		else
			CurrentStateClass.GetDefaultObject()->InputActionMediumCrouch(this);
		//}
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
void UInputParserState::InputActionQCF(UInputParserComponent * Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionQCF()")); }
void UInputParserState::InputActionQCB(UInputParserComponent * Parser) { UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserState::InputActionQCB()")); }


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
	UE_LOG(LogUInputParser, Error, TEXT("UInputParserDefaultState::InputActionLeft()"));
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

void UInputParserDefaultState::InputActionQCF(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionQCF()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_QCF);
	}
}

void UInputParserDefaultState::InputActionQCB(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UInputParserDefaultState::InputActionQCB()"));
	if (Parser != nullptr)
	{
		Parser->PushInputToken(EInputToken::IE_QCB);
	}
}

bool UPreLeftDashState::IsComplex() { return true; }

void UPreLeftDashState::InputActionLeft(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionLeft()"));
	if (Parser != nullptr)
	{
		//if (!Cast<AFighterCharacter>(Parser->GetOwner())->IsToRightOfOpponent() || (Cast<AFighterCharacter>(Parser->GetOwner())->IsToRightOfOpponent() && (Parser->GetFighterFacing() > 0)))
		if(Parser->GetFighterFacing() > 0)
		{
			UE_LOG(LogUInputParser, Error, TEXT("is left of opponent, dash back"));
			Parser->PushInputToken(EInputToken::IE_DashBackward);
		}
		else
		{
			UE_LOG(LogUInputParser, Error, TEXT("is right of opponent, dash forward"));
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

void UPreLeftDashState::InputActionQCF(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionQCF()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionQCF(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreLeftDashState::InputActionQCB(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreLeftDashState::InputActionQCB()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionQCB(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

bool UPreRightDashState::IsComplex() { return true; }

void UPreRightDashState::InputActionRight(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionRight()"));
	if (Parser != nullptr)
	{
		//if (!Cast<AFighterCharacter>(Parser->GetOwner())->IsToRightOfOpponent())
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

void UPreRightDashState::InputActionQCF(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionQCF()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionQCF(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}

void UPreRightDashState::InputActionQCB(UInputParserComponent *Parser)
{
	UE_LOG(LogUInputParser, Verbose, TEXT("UPreRightDashState::InputActionQCB()"));
	if (Parser != nullptr)
	{
		UInputParserDefaultState::InputActionQCB(Parser);
		ChangeState(Parser, UInputParserDefaultState::StaticClass());
	}
}