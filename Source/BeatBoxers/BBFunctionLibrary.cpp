// copyright 2017 BYU Animation

#include "BBFunctionLibrary.h"
#include "GameFramework/Actor.h"

UBBFunctionLibrary::UBBFunctionLibrary(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FVector UBBFunctionLibrary::GetWorldPosition(AActor* Reference, FVector RelativePosition)
{
	if (Reference == nullptr)
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBFunctionLibrary passed invalid actor reference in GetRelativePosition."));
		return RelativePosition;
	}
	if (Reference->IsPendingKill())
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBFunctionLibrary passed actor %s, which is pending kill."), *GetNameSafe(Reference));
		return RelativePosition;
	}

	return Reference->GetActorTransform().TransformPosition(RelativePosition);
}