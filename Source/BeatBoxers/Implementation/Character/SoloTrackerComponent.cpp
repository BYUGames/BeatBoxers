// copyright 2017 BYU Animation

#include "SoloTrackerComponent.h"


// Sets default values for this component's properties
USoloTrackerComponent::USoloTrackerComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UBasicFretboardFeed* USoloTrackerComponent::GetMyFretboardFeed()
{
	if (MyFretboardFeed == nullptr)
	{
		MyFretboardFeed = NewObject<UBasicFretboardFeed>();
		MyFretboardFeed->SetTimerManager(GetOwner()->GetWorldTimerManager());
	}
	return MyFretboardFeed;
}

// Called when the game starts
void USoloTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void USoloTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USoloTrackerComponent::RegisterFighterWorld(TWeakObjectPtr<UObject> FighterWorld)
{
	if (!FighterWorld.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s USoloTrackerComponent given invalid object to register as FighterWorld."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighterWorld = Cast<IFighterWorld>(FighterWorld.Get());
		if (MyFighterWorld == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s USoloTrackerComponent given %s to register as FighterWorld, but it doesn't implement IFighterWorld."), *GetNameSafe(GetOwner()), *GetNameSafe(FighterWorld.Get()));
		}
	}
}

void USoloTrackerComponent::RegisterFighter(TWeakObjectPtr<UObject> Fighter)
{
	if (!Fighter.IsValid())
	{
		UE_LOG(LogUFighterState, Error, TEXT("%s USoloTrackerComponent given invalid object to register as Fighter."), *GetNameSafe(GetOwner()));
	}
	else
	{
		MyFighter = Cast<IFighter>(Fighter.Get());
		if (MyFighter == nullptr)
		{
			UE_LOG(LogUFighterState, Error, TEXT("%s USoloTrackerComponent given %s to register as Fighter, but it doesn't implement IFighter."), *GetNameSafe(GetOwner()), *GetNameSafe(Fighter.Get()));
		}
	}
}

void USoloTrackerComponent::BeginSolo(FSoloParameters SoloParameters)
{
	//TODO
}

void USoloTrackerComponent::EndSolo()
{
	ClearFeed();
	//TODO
}
