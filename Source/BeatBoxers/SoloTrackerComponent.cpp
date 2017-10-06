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

void USoloTrackerComponent::BeginSolo(FSoloParameters SoloParameters)
{
	//TODO
}

void USoloTrackerComponent::EndSolo()
{
	//TODO
}