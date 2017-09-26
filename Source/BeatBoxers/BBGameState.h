// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Camera/CameraActor.h"
#include "BBGameState.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBGameState : public AGameState
{
	GENERATED_UCLASS_BODY()
	
public:
	ACameraActor* MainCamera;
};
