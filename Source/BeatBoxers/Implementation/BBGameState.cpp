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
	if (WorldMusicBox == nullptr)
	{
		ABBGameMode *GameMode = Cast<ABBGameMode>(AuthorityGameMode);
		if (GameMode != nullptr)
		{
			if (GameMode->DefaultMusicBoxClass != nullptr)
			{
				WorldMusicBox = GetWorld()->SpawnActor(GameMode->DefaultMusicBoxClass);
				if (!GetIMusicBox())
				{
					UE_LOG(LogGameMode, Error, TEXT("Gamemode DefaultMusicBox class does not implement IMusicBox."));
				}
			}
			else
			{
				UE_LOG(LogGameMode, Error, TEXT("Gamemode DefaultMusicBox class is nullptr."));
			}
		}
		else
		{
			UE_LOG(LogGameMode, Error, TEXT("Gamestate unable to cast gamemode to ABBGameMode."));
		}
	}
	return WorldMusicBox;
}

AActor* ABBGameState::GetAMusicBox()
{
	return Cast<AActor>(GetUMusicBox());
}

IMusicBox* ABBGameState::GetIMusicBox()
{
	return Cast<IMusicBox>(GetUMusicBox());
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