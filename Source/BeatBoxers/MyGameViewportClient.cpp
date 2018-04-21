// copyright 2017 BYU Animation

#include "MyGameViewportClient.h"
#include "Engine/Console.h"
#include "Implementation/BBPlayerController.h"



bool UMyGameViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	// Propagate keyboard events to all players
	UEngine* const Engine = GetOuterUEngine();
	int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
	bool bRetVal = false;
	for (int32 i = 0; i < NumPlayers; i++)
	{
		int32 j = i;
		//bRetVal = Super::InputKey(Viewport, i, Key, EventType, AmountDepressed, bGamepad) || bRetVal;

		if (IgnoreInput())
		{
			return false;
		}

		if ((Key == EKeys::Enter && EventType == EInputEvent::IE_Pressed && FSlateApplication::Get().GetModifierKeys().IsAltDown() && GetDefault<UInputSettings>()->bAltEnterTogglesFullscreen)
			|| (IsRunningGame() && Key == EKeys::F11 && EventType == EInputEvent::IE_Pressed && GetDefault<UInputSettings>()->bF11TogglesFullscreen))
		{
			HandleToggleFullscreenCommand();
			return true;
		}

		const int32 NumLocalPlayers = World->GetGameInstance()->GetNumLocalPlayers();

		if (NumLocalPlayers > 1 && Key.IsGamepadKey() && GetDefault<UGameMapsSettings>()->bOffsetPlayerGamepadIds)
		{
			++j;
		}




		// route to subsystems that care
		bool bResult = (ViewportConsole ? ViewportConsole->InputKey(j, Key, EventType, AmountDepressed, bGamepad) : false);

		if (!bResult)
		{
			ULocalPlayer* const TargetPlayer = GEngine->GetLocalPlayerFromControllerId(this, j);
			if (TargetPlayer && TargetPlayer->PlayerController)
			{
				bResult = Cast<ABBPlayerController>(TargetPlayer->PlayerController)->CustomInputKey(ControllerId, Key, EventType, AmountDepressed, bGamepad);

				if (IgnoreInput())
				{
					return ViewportConsole ? ViewportConsole->InputKey(ControllerId, Key, EventType, AmountDepressed, bGamepad) : false;
				}

				if ((Key == EKeys::Enter && EventType == EInputEvent::IE_Pressed && FSlateApplication::Get().GetModifierKeys().IsAltDown() && GetDefault<UInputSettings>()->bAltEnterTogglesFullscreen)
					|| (IsRunningGame() && Key == EKeys::F11 && EventType == EInputEvent::IE_Pressed && GetDefault<UInputSettings>()->bF11TogglesFullscreen))
				{
					HandleToggleFullscreenCommand();
					return true;
				}

				const int32 NumLocalPlayers = World->GetGameInstance()->GetNumLocalPlayers();

				if (NumLocalPlayers > 1 && Key.IsGamepadKey() && GetDefault<UGameMapsSettings>()->bOffsetPlayerGamepadIds)
				{
					++ControllerId;
				}


				// route to subsystems that care
				bool bResult = (ViewportConsole ? ViewportConsole->InputKey(ControllerId, Key, EventType, AmountDepressed, bGamepad) : false);

				if (!bResult)
				{
					ULocalPlayer* const TargetPlayer = GEngine->GetLocalPlayerFromControllerId(this, ControllerId);
					if (TargetPlayer && TargetPlayer->PlayerController)
					{
						bResult = Cast<ABBPlayerController>(TargetPlayer->PlayerController)->CustomInputKey(ControllerId, Key, EventType, AmountDepressed, bGamepad);
					}

					// A gameviewport is always considered to have responded to a mouse buttons to avoid throttling
					if (!bResult && Key.IsMouseButton())
					{
						bResult = true;
					}
				}


			}

			// A gameviewport is always considered to have responded to a mouse buttons to avoid throttling
			if (!bResult && Key.IsMouseButton())
			{
				bResult = true;
			}
		}

	}

	return bRetVal;
}

bool UMyGameViewportClient::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	// Propagate keyboard events to all players
	UEngine* const Engine = GetOuterUEngine();
	int32 const NumPlayers = Engine ? Engine->GetNumGamePlayers(this) : 0;
	bool bRetVal = false;
	for (int32 i = 0; i < NumPlayers; i++)
	{
		bRetVal = Super::InputAxis(Viewport, i, Key, Delta, DeltaTime, NumSamples, bGamepad) || bRetVal;
	}

	return bRetVal;

}
