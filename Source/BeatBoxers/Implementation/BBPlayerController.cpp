// copyright 2017 BYU Animation

#include "BBPlayerController.h"
#include "BBGameState.h"

ABBPlayerController::ABBPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UBasicFretboard* ABBPlayerController::GetBGFretboard()
{
	if (BGFretboard == nullptr)
	{
		BGFretboard = NewObject<UBasicFretboard>();
		BGFretboard->SetTimerManager(GetWorldTimerManager());

		//TODO: Fix this sketchy code
		BGFretboard->Listen(GetWorld()->GetGameState<ABBGameState>()->GetMusicBox());
	}
	return BGFretboard;
}