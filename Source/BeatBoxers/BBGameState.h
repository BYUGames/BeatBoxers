// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Camera/CameraActor.h"
#include "IMusicBox.h"
#include "BBGameState.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBGameState : public AGameState
{
	GENERATED_UCLASS_BODY()
	
public:
	IMusicBox *WorldMusicBox;
	ACameraActor *MainCamera;
	TMap<AController*, float> Scores;

	UFUNCTION(BlueprintCallable)
	float GetScore(AController* Controller);

	float AddScore(AController* Controller, float Amount);
	float SetScore(AController* Controller, float Value);
	void ResetScores();
};
