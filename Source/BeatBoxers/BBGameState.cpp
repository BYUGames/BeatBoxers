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
