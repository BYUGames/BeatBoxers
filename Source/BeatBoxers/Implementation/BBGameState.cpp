// copyright 2017 BYU Animation

#include "BBGameState.h"

ABBGameState::ABBGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WorldMusicBox = nullptr;
}

AActor* ABBGameState::GetMusicBox()
{
	return Cast<AActor>(WorldMusicBox);
}

IMusicBox* ABBGameState::GetIMusicBox()
{
	return WorldMusicBox;
}

void ABBGameState::SetMusicBox(IMusicBox *NewMusicBox)
{
	if (WorldMusicBox != nullptr)
	{
		WorldMusicBox->GetOnBeatEvent().RemoveDynamic(this, &ABBGameState::OnBeat);
		WorldMusicBox->GetMusicEndEvent().RemoveDynamic(this, &ABBGameState::OnMusicEnd);
	}
	WorldMusicBox = NewMusicBox;
	if (WorldMusicBox != nullptr)
	{
		WorldMusicBox->GetOnBeatEvent().AddDynamic(this, &ABBGameState::OnBeat);
		WorldMusicBox->GetMusicEndEvent().AddDynamic(this, &ABBGameState::OnMusicEnd);
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