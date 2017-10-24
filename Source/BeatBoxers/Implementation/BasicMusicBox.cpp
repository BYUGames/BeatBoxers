// copyright 2017 BYU Animation

#include "BasicMusicBox.h"


// Sets default values
ABasicMusicBox::ABasicMusicBox(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

UBasicFretboardFeed* ABasicMusicBox::GetFretboardFeed()
{
	if (MyFretboardFeed == nullptr)
	{
		MyFretboardFeed = NewObject<UBasicFretboardFeed>();
		MyFretboardFeed->SetTimerManager(GetWorldTimerManager());
	}
	return MyFretboardFeed;
}

FBeatEvent& ABasicMusicBox::GetOnBeatEvent()
{
	return BeatEvent;
}

FMusicEndEvent& ABasicMusicBox::GetMusicEndEvent()
{
	return MusicEndEvent;
}

void ABasicMusicBox::DropBeat()
{
	if (BeatEvent.IsBound())
	{
		BeatEvent.Broadcast();
	}
}

void ABasicMusicBox::EndMusic()
{
	if (MusicEndEvent.IsBound())
	{
		UE_LOG(LogBeatBoxers, Log, TEXT("calling MusicEndEvent"));
		MusicEndEvent.Broadcast();
	}
	else
	{
		UE_LOG(LogBeatBoxers, Log, TEXT("MusicEndEvent not bound"));
	}
}

int ABasicMusicBox::StartMusic()
{
	K2_StartMusic();
	return true;
}

int ABasicMusicBox::PauseMusic()
{
	K2_PauseMusic();
	return true;
}

int ABasicMusicBox::ResumeMusic()
{
	K2_ResumeMusic();
	return true;
}

int ABasicMusicBox::StopMusic()
{
	K2_StopMusic();
	return true;
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

int ABasicMusicBox::K2_StartMusic_Implementation()
{
	//NOP
	return true;
}

int ABasicMusicBox::K2_StopMusic_Implementation()
{
	ClearFeed();
	return true;
}

int ABasicMusicBox::K2_PauseMusic_Implementation()
{
	PauseFeed();
	return true;
}

int ABasicMusicBox::K2_ResumeMusic_Implementation()
{
	ResumeFeed();
	return true;
}