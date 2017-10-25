// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Camera/CameraActor.h"
#include "Interfaces/IMusicBox.h"
#include "BBGameState.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY()
	UObject *WorldMusicBox;

	UFUNCTION()
	void OnBeat();

	UFUNCTION()
	void OnMusicEnd();

public:
	UPROPERTY(BlueprintAssignable)
	FBeatEvent BeatEvent;

	UPROPERTY(BlueprintAssignable)
	FMusicEndEvent MusicEndEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraActor *MainCamera;

	UObject* GetUMusicBox();

	UFUNCTION(BlueprintCallable)
	AActor* GetAMusicBox();

	IMusicBox* GetIMusicBox();

	void SetMusicBox(UObject *NewMusicBox);
};
