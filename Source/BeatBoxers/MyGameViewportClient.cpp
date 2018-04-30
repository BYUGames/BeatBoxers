// copyright 2017 BYU Animation

#include "MyGameViewportClient.h"
#include "Engine/Console.h"
#include "Implementation/BBPlayerController.h"
#include "Implementation/BBGameMode.h"


bool UMyGameViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	auto GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode != nullptr)
	{
		auto BBGameMode = Cast<ABBGameMode>(GameMode);
		if (BBGameMode != nullptr)
		{
			BBGameMode->SkipIntro();
		}
	}
	bool bRetVal = false;


	int P1ControlScheme = GetWorld()->GetGameInstance<UBBGameInstance>()->P1ControlScheme;
	int P2ControlScheme = GetWorld()->GetGameInstance<UBBGameInstance>()->P2ControlScheme;

	if ((P1ControlScheme == 4 && P2ControlScheme == 5) || (P1ControlScheme == 0 && P1ControlScheme == 0)) {
		bRetVal = Super::InputKey(Viewport, ControllerId, Key, EventType, AmountDepressed, bGamepad) || bRetVal;
	} else if (P1ControlScheme == 5 && P1ControlScheme == 4) {
		bRetVal = Super::InputKey(Viewport, (1-ControllerId), Key, EventType, AmountDepressed, bGamepad) || bRetVal;
	}
	else {
		// Propagate keyboard events to all players
		UEngine* const Engine = GetOuterUEngine();
		int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
		
		for (int32 i = 0; i < NumPlayers; i++)
		{
			bRetVal = Super::InputKey(Viewport, i, Key, EventType, AmountDepressed, bGamepad) || bRetVal;
		}

	}
	return bRetVal;

	
}

bool UMyGameViewportClient::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	
	if (bGamepad) {
		GetWorld()->GetGameInstance<UBBGameInstance>()->FirstControllerPluggedIn = true;
	}
	if (ControllerId == 1) {
		GetWorld()->GetGameInstance<UBBGameInstance>()->SecondControllerPluggedIn = true;
	}

	bool bRetVal = false;
	int P1ControlScheme = GetWorld()->GetGameInstance<UBBGameInstance>()->P1ControlScheme;
	int P2ControlScheme = GetWorld()->GetGameInstance<UBBGameInstance>()->P2ControlScheme;

	if ((P1ControlScheme ==4 && P2ControlScheme == 5) || (P1ControlScheme == 0 && P1ControlScheme == 0)) {
		bRetVal = Super::InputAxis(Viewport, ControllerId, Key, Delta, DeltaTime, NumSamples, bGamepad) || bRetVal;
	}
	else if (P1ControlScheme == 5 && P2ControlScheme == 4) {
		bRetVal = Super::InputAxis(Viewport, (1 - ControllerId), Key, Delta, DeltaTime, NumSamples, bGamepad) || bRetVal;
	}
	else {
		// Propagate keyboard events to all players
		UEngine* const Engine = GetOuterUEngine();
		int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
		
		for (int32 i = 0; i < NumPlayers; i++)
		{
			bRetVal = Super::InputAxis(Viewport, i, Key, Delta, DeltaTime, NumSamples, bGamepad) || bRetVal;
		}
	}
	return bRetVal;

}
