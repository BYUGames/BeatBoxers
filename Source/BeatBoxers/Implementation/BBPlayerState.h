// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()
	
public:
	APawn *MyPawn;
};
