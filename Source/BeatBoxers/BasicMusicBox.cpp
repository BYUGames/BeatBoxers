// copyright 2017 BYU Animation

#include "BasicMusicBox.h"


// Sets default values
ABasicMusicBox::ABasicMusicBox(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TimeBetweenBeats = 1.f;
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

void ABasicMusicBox::DropBeat()
{
	if (BeatEvent.IsBound())
	{
		BeatEvent.Broadcast();
	}
}

void ABasicMusicBox::StartMusic()
{
	GetWorldTimerManager().SetTimer(
		TimerHandle_Beat,
		this,
		&ABasicMusicBox::DropBeat,
		TimeBetweenBeats,
		true
	);
}

void ABasicMusicBox::PauseMusic()
{
	GetWorldTimerManager().PauseTimer(TimerHandle_Beat);
}

void ABasicMusicBox::ResumeMusic()
{
	GetWorldTimerManager().UnPauseTimer(TimerHandle_Beat);
}

void ABasicMusicBox::StopMusic()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Beat);
}

float ABasicMusicBox::GetTimeToNextBeat()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Beat))
	{
		return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Beat);
	}
	return -TimeBetweenBeats;
}