// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxers.h"
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

protected:
	UDataTable *MoveData;

	virtual void LoadData(FString CSVPath, UDataTable *DataTable);
	
public:
	/** Data used to set up next game. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNewGameData NewGameData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int P1ControlScheme; //1 is keyboard1, 2 is keyboard2, 3 is generic controller, 4 is xbox controller 1, 5 is xbox controller 2
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int P2ControlScheme; //1 is keyboard1, 2 is keyboard2, 3 is generic controller, 4 is xbox controller 1, 5 is xbox controller 2

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1HorizontalDefaultValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P2HorizontalDefaultValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1VerticalDefaultValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P2VerticalDefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisUpValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisUpRightValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisRightValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisBottomRightValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisBottomValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisBottomLeftValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisLeftValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float P1360axisTopLeftValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisUpValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisUpRightValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisRightValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisBottomRightValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisBottomValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisBottomLeftValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisLeftValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float P2360axisTopLeftValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool P1360Axis;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool P21360Axis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FirstControllerPluggedIn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SecondControllerPluggedIn;

	virtual void Init() override;
};
