// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "Blueprint/UserWidget.h"
#include "FretboardNoteWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API UFretboardNoteWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int NoteID;

	UPROPERTY(BlueprintReadOnly)
	FNoteWidgetData NoteWidgetData;

	/** As a result of this function, this widget should eventually call RemoveFromParent(). That is all the default implementation does. Item stops being updated by FretboardWidget and is removed from its map immediately. */
	UFUNCTION(BlueprintNativeEvent)
	void OnNoteEnded(float Accuracy);

	/** As a result of this function, this widget should eventually call RemoveFromParent(). That is all the default implementation does. Item stops being updated by FretboardWidget and is removed from its map immediately. */
	UFUNCTION(BlueprintNativeEvent)
	void OnNoteCleared();
};
