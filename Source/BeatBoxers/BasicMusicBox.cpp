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

FMusicEndEvent& ABasicMusicBox::GetMusicEndEvent()
{
	return MusicEndEvent;
}

void ABasicMusicBox::OnNewNote(FNoteData data)
{
	if (NewNoteEvent.IsBound())
	{
		NewNoteEvent.Broadcast(data);
	}
}

void ABasicMusicBox::EndMusic()
{
	if (MusicEndEvent.IsBound()) 
	{
		MusicEndEvent.Broadcast();
	}
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
	K2_StartMusic();
}

void ABasicMusicBox::PauseMusic()
{
	K2_PauseMusic();
}

void ABasicMusicBox::ResumeMusic()
{
	K2_ResumeMusic();
}

void ABasicMusicBox::StopMusic()
{
	K2_StopMusic();
}

float ABasicMusicBox::GetTimeToSongEnd()
{
	return K2_GetTimeToSongEnd();
}

float ABasicMusicBox::GetTimeToNextBeat()
{
	return K2_GetTimeToNextBeat();
}

float ABasicMusicBox::K2_GetTimeToSongEnd_Implementation() const
{
	//NOP
	return 0.f;
}

float ABasicMusicBox::K2_GetTimeToNextBeat_Implementation() const
{
	//NOP
	return 0.f;
}

void ABasicMusicBox::K2_StartMusic_Implementation() const
{
	//NOP
}

void ABasicMusicBox::K2_StopMusic_Implementation() const
{
	//NOP
}

void ABasicMusicBox::K2_PauseMusic_Implementation() const
{
	//NOP
}

void ABasicMusicBox::K2_ResumeMusic_Implementation() const
{
	//NOP
}