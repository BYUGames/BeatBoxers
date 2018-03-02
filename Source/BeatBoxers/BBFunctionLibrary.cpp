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

ENoteType UBBFunctionLibrary::GetNoteFromInput(EInputToken InputToken)
{
	switch (InputToken)
	{
	case EInputToken::IE_Light:
		return ENoteType::NE_Light;
	case EInputToken::IE_Medium:
		return ENoteType::NE_Medium;
	case EInputToken::IE_Heavy:
		return ENoteType::NE_Heavy;
	case EInputToken::IE_Up:
		return ENoteType::NE_Up;
	case EInputToken::IE_Down:
		return ENoteType::NE_Down;
	case EInputToken::IE_Left:
		return ENoteType::NE_Left;
	case EInputToken::IE_Right:
		return ENoteType::NE_Right;
	default:
		return ENoteType::NE_None;
	}
}

UBBGameUserSettings* UBBFunctionLibrary::GetBBGameUserSettings()
{
	if (GEngine != nullptr)
	{
		if (GEngine->GetGameUserSettings() != nullptr)
		{
			return Cast<UBBGameUserSettings>(GEngine->GetGameUserSettings());
		}
	}
	return nullptr;
}