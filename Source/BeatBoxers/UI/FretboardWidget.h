// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "BeatBoxersStructs.h"
#include "Interfaces/IFretboard.h"
#include "Blueprint/UserWidget.h"
#include "FretboardNoteWidget.h"
#include "FretboardWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API UFretboardWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
	
protected:
	IFretboard* MyFretboard;

	UPROPERTY()
	TMap<int, UFretboardNoteWidget*> Notes;

	UFUNCTION()
	virtual void OnNewNote(int NoteID, FNoteData NoteData);

	UFUNCTION()
	virtual void OnNoteEnd(int NoteID, float Accuracy);

	UFUNCTION()
	virtual void OnNotesCleared();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UFretboardNoteWidget> NoteWidgetClass;

	UFUNCTION(BlueprintCallable)
	int BindToFretboard(UObject* Fretboard);

	UFUNCTION(BlueprintCallable)
	void UnbindFromFretboard();

	UFUNCTION(BlueprintPure)
	const TMap<int, UFretboardNoteWidget*> GetNotes() { return Notes; }

	/** Called whenever a new note has been created. Provides a hook for blueprint implementation to attach this widget where ever it needs to be. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="On New Note"))
	void K2_OnNewNote(UFretboardNoteWidget* NoteWidget);

	/** Called whenever a note has ended. Provides a hook for blueprint implementation to do whatever, but the responsibility of dying is the note's. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="On Note Ended"))
	void K2_OnNoteEnded(UFretboardNoteWidget* NoteWidget, float Accuracy);

	/** Called whenever all notes were cleared. Provides a hook for blueprint implementation to do whatever, but the responsibility of dying is the note's. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="On Notes Cleared"))
	void K2_OnNotesCleared();

	/** Called whenever a note is updated. Provides a hook for blueprint implementation update the note's visuals. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="On Note Updated"))
	void K2_OnNoteUpdated(UFretboardNoteWidget* NoteWidget);
};
