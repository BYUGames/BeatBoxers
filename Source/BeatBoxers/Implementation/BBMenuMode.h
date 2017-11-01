// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxers.h"
#include "GameFramework/GameMode.h"
#include "BBGameInstance.h"
#include "BBMenuMode.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBMenuMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerCharacter(int PlayerIndex, TSubclassOf<AActor> Character);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AActor> GetPlayerCharacter(int PlayerIndex);

	UFUNCTION(BlueprintCallable)
	void SetLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	FName GetLevel();

	UFUNCTION(BlueprintCallable)
	void SetSong(FName SongName);

	UFUNCTION(BlueprintCallable)
	FName GetSong();

	UFUNCTION(BlueprintCallable)
	void StartGame();
};
