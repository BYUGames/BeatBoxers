// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "BBPlayerStart.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BEATBOXERS_API ABBPlayerStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()
	
protected:
	/** The ID of the player that is allowed to spawn here. 0, 1, or -1 for either. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerID;

public:
	int32 GetPlayerID();
};
