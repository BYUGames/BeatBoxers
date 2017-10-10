// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BeatBoxersStructs.h"
#include "BBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API UBBGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Data used to set up next game. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNewGameData NewGameData;
};
