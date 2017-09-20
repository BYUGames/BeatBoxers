// copyright 2017 BYU Animation

#include "SoloTrackerComponent.h"


// Sets default values for this component's properties
USoloTrackerComponent::USoloTrackerComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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

void USoloTrackerComponent::RegisterMoveset(TWeakObjectPtr<UObject> Moveset)
{
	if (!Moveset.IsValid())
	{
		UE_LOG(BeatBoxersLog, Error, TEXT("USoloTrackerComponent %s given invalid object to register as Moveset."), *GetNameSafe(this));
	}
	else
	{
		MyMoveset = Cast<IMoveset>(Moveset.Get());
		if (MyMoveset == nullptr)
		{
			UE_LOG(BeatBoxersLog, Error, TEXT("USoloTrackerComponent %s given %s to register as Moveset, but it doesn't implement IMoveset."), *GetNameSafe(this), *GetNameSafe(Moveset.Get()));
		}
	}
}

void USoloTrackerComponent::BeginSolo(FSoloParameters SoloParameters)
{
	//TODO
}

void USoloTrackerComponent::ReceiveInputToken(EInputToken Input)
{
	//TODO
}