// copyright 2017 BYU Animation

#include "InputParserComponent.h"


// Sets default values for this component's properties
UInputParserComponent::UInputParserComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInputParserComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
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
		UE_LOG(BeatBoxersLog, Error, TEXT("UInputParserComponent %s given invalid object to register as FighterState."), *GetNameSafe(this));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(FighterState.Get());
		if (MyFighterState == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UInputParserComponent %s given %s to register as FighterState, but it doesn't implement IFighterState."), *GetNameSafe(this), *GetNameSafe(FighterState.Get()));
		}
	}
}

void UInputParserComponent::RegisterMoveset(TWeakObjectPtr<UObject> Moveset)
{
	if (!Moveset.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UInputParserComponent %s given invalid object to register as Moveset."), *GetNameSafe(this));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset.Get());
		if (MyMoveset == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UInputParserComponent %s given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(this), *GetNameSafe(Moveset.Get()));
		}
	}
}

void UInputParserComponent::OnControlReturned()
{
	//TODO
}

void UInputParserComponent::InputAxisHorizontal(float Amount)
{
	//TODO
}

void UInputParserComponent::InputAxisVertical(float Amount)
{
	//TODO
}

void UInputParserComponent::InputActionDLeft(bool IsUp)
{
	//TODO
}

void UInputParserComponent::InputActionDRight(bool IsUp)
{
	//TODO
}

void UInputParserComponent::InputActionDDown(bool IsUp)
{
	//TODO
}

void UInputParserComponent::InputActionDUp(bool IsUp)
{
	//TODO
}

void UInputParserComponent::InputActionPunch(bool IsUp)
{
	//TODO
}

void UInputParserComponent::InputActionKick(bool IsUp)
{
	//TODO
}