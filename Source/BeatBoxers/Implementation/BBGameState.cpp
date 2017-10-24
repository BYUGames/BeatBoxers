// copyright 2017 BYU Animation

#include "BBGameState.h"
#include "BBGameMode.h"
#include "Runtime/Engine/Classes/Engine/World.h"

ABBGameState::ABBGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WorldMusicBox = nullptr;
}

UObject* ABBGameState::GetUMusicBox()
{
	return WorldMusicBox;
}

AActor* ABBGameState::GetAMusicBox()
{
	return Cast<AActor>(WorldMusicBox);
}

IMusicBox* ABBGameState::GetIMusicBox()
{
	return Cast<IMusicBox>(WorldMusicBox);
}

void ABBGameState::SetMusicBox(UObject *NewMusicBox)
{
	if (GetIMusicBox() != nullptr)
	{
		GetIMusicBox()->GetOnBeatEvent().RemoveDynamic(this, &ABBGameState::OnBeat);
		GetIMusicBox()->GetMusicEndEvent().RemoveDynamic(this, &ABBGameState::OnMusicEnd);
	}
	WorldMusicBox = NewMusicBox;
	if (GetIMusicBox() != nullptr)
	{
		GetIMusicBox()->GetOnBeatEvent().AddDynamic(this, &ABBGameState::OnBeat);
		GetIMusicBox()->GetMusicEndEvent().AddDynamic(this, &ABBGameState::OnMusicEnd);
	}
}

void ABBGameState::OnBeat()
{
	if (BeatEvent.IsBound())
	{
		BeatEvent.Broadcast();
	}
}

void ABBGameState::OnMusicEnd()
{
	if (MusicEndEvent.IsBound())
	{
		MusicEndEvent.Broadcast();
	}
}