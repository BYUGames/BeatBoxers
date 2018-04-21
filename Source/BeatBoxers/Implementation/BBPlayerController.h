// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BeatBoxersStructs.h"
#include "BasicFretboard.h"
#include "BBPlayerController.generated.h"

/**
 *
 */
UCLASS()
class BEATBOXERS_API ABBPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	public:
		bool CustomInputKey(int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad);

 };
