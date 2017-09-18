// Fill out your copyright notice in the Description page of Project Settings.

#include "MovesetComponent.h"


// Sets default values for this component's properties
UMovesetComponent::UMovesetComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMovesetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMovesetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMovesetComponent::RegisterFighterState(class UObject* FighterState)
{
	if (FighterState == nullptr)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UMovesetComponent %s given nullptr to register as FighterState."), *GetNameSafe(this));
	}
	else
	{
		MyFighterState = Cast<IFighterState>(FighterState);
		if (MyFighterState == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UMovesetComponent %s given %s to register as FighterState, but it doesn't implement IFighterState."), *GetNameSafe(this), *GetNameSafe(FighterState));
		}
	}
}

void UMovesetComponent::RegisterSoloTracker(class UObject* SoloTracker)
{
	if (SoloTracker == nullptr)
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("UMovesetComponent %s given nullptr to register as SoloTracker."), *GetNameSafe(this));
	}
	else
	{
		MySoloTracker = Cast<ISoloTracker>(SoloTracker);
		if (MySoloTracker == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("UMovesetComponent %s given %s to register as SoloTracker, but it doesn't implement ISoloTracker."), *GetNameSafe(this), *GetNameSafe(SoloTracker));
		}
	}
}

void UMovesetComponent::ReceiveInputToken(EInputToken Token)
{
	//TODO
}

void UMovesetComponent::OnWindowFinished(bool WasInterrupted)
{
	//TODO
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