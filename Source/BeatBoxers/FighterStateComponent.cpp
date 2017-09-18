// Fill out your copyright notice in the Description page of Project Settings.

#include "FighterStateComponent.h"


// Sets default values for this component's properties
UFighterStateComponent::UFighterStateComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFighterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFighterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFighterStateComponent::RegisterFighterWorld(class UObject* FighterWorld)
{
	if (FighterWorld == nullptr)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given nullptr to register as FighterWorld."), *GetNameSafe(this));
	}
	else
	{
		MyFighterWorld = Cast<IFighterWorld>(FighterWorld);
		if (MyFighterWorld == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as FighterWorld, but it doesn't implement IFighterWorld."), *GetNameSafe(this), *GetNameSafe(FighterWorld));
		}
	}
}

void UFighterStateComponent::RegisterFighter(class UObject* Fighter)
{
	if (Fighter == nullptr)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given nullptr to register as Fighter."), *GetNameSafe(this));
	}
	else
	{
		MyFighter = Cast<IFighter>(Fighter);
		if (MyFighter == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as Fighter, but it doesn't implement IFighter."), *GetNameSafe(this), *GetNameSafe(Fighter));
		}
	}
}

void UFighterStateComponent::RegisterMoveset(class UObject* Moveset)
{
	if (Moveset == nullptr)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given nullptr to register as Moveset."), *GetNameSafe(this));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset);
		if (MyMoveset == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(this), *GetNameSafe(Moveset));
		}
	}
}

void UFighterStateComponent::RegisterInputParser(class UObject* InputParser)
{
	if (InputParser == nullptr)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given nullptr to register as InputParser."), *GetNameSafe(this));
	}
	else
	{
		MyInputParser = Cast<IInputParser>(InputParser);
		if (MyInputParser == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UFighterStateComponent %s given %s to register as InputParser, but it doesn't implement IInputParser."), *GetNameSafe(this), *GetNameSafe(InputParser));
		}
	}
}

bool UFighterStateComponent::IsInputBlocked() const
{
	//TODO
	return false;
}

bool UFighterStateComponent::IsBlocking() const
{
	//TODO
	return false;
}

void UFighterStateComponent::StartMoveWindow(FMoveWindow& Window, bool IsLastInSequence)
{
	//TODO
}

void UFighterStateComponent::StartStun(float Duration)
{
	//TODO
}

void UFighterStateComponent::SetMoveDirection(float Direction)
{
	//TODO
}

void UFighterStateComponent::SetWantsToCrouch(bool WantsToCrouch)
{
	//TODO
}

void UFighterStateComponent::Jump()
{
	//TODO
}
