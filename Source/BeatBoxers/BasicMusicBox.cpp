// copyright 2017 BYU Animation

#include "BasicMusicBox.h"


// Sets default values
ABasicMusicBox::ABasicMusicBox(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FBeatEvent& ABasicMusicBox::GetOnBeatEvent()
{
	return BeatEvent;
}

TArray<FNoteData> ABasicMusicBox::GetExistingNotes_Implementation()
{
	return TArray<FNoteData>();
}

FNewNoteEvent& ABasicMusicBox::GetOnNewNoteEvent()
{
	return NewNoteEvent;
}