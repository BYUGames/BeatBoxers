// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "BBGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(config=GameUserSettings, configdonotcheckdefaults)
class BEATBOXERS_API UBBGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()
	
public:
	// Changes crowd detail level
	// @param Value 0:none, 1:low, 2:medium, 3:high
	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetCrowdDetailLevel(int32 Value);

	UFUNCTION(BlueprintPure, Category = Settings)
	int32 GetCrowdDetailLevel() const;

	void SetOverallScalabilityLevel(int32 Value) override;

protected:
	UPROPERTY(config, BlueprintReadOnly)
	int32 CrowdDetailLevel;
};
