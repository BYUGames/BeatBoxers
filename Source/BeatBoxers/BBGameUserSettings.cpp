// copyright 2017 BYU Animation

#include "BBGameUserSettings.h"

UBBGameUserSettings::UBBGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UBBGameUserSettings::SetCrowdDetailLevel(int32 Value)
{
	CrowdDetailLevel = FMath::Clamp(Value, 0, 3);
}

int32 UBBGameUserSettings::GetCrowdDetailLevel() const
{
	return CrowdDetailLevel;
}

void UBBGameUserSettings::SetOverallScalabilityLevel(int32 Value)
{
	UE_LOG(LogBeatBoxers, Warning, TEXT("Saving Overall Scalability %i"), Value);
	Super::SetOverallScalabilityLevel(Value);
	SetCrowdDetailLevel(Value);
}