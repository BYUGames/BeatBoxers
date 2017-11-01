// copyright 2017 BYU Animation

#include "BBMenuMode.h"
#include "Kismet/GameplayStatics.h"

ABBMenuMode::ABBMenuMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ABBMenuMode::SetPlayerCharacter(int PlayerIndex, TSubclassOf<AActor> Character)
{
	//Hardcoded two player
	if (PlayerIndex <= 1 && GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		if (PlayerIndex == 0)
		{
			GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.Player0Class = Character;
		}
		else
		{
			GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.Player1Class = Character;
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to set player %d character in gameinstance."), PlayerIndex);
	}
}

TSubclassOf<AActor> ABBMenuMode::GetPlayerCharacter(int PlayerIndex)
{
	//Hardcoded two player
	if (PlayerIndex <= 1 && GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		if (PlayerIndex == 0)
		{
			return GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.Player0Class;
		}
		else
		{
			return GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.Player1Class;
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to get player %d character in gameinstance."), PlayerIndex);
		return AActor::StaticClass();
	}
}

void ABBMenuMode::SetLevel(FName LevelName)
{
	if (GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.LevelName = LevelName;
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to set level in gameinstance."));
	}
}

FName ABBMenuMode::GetLevel()
{
	if (GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		return GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.LevelName;
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to get level in gameinstance."));
		return FName();
	}
}

void ABBMenuMode::SetSong(FName SongName)
{
	if (GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.SongName = SongName;
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to set song in gameinstance."));
	}
}

FName ABBMenuMode::GetSong()
{
	if (GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		return GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.SongName;
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to get song in gameinstance."));
		return FName();
	}
}

void ABBMenuMode::StartGame()
{
	if (GetWorld() != nullptr && GetWorld()->GetGameInstance<UBBGameInstance>() != nullptr)
	{
		UGameplayStatics::OpenLevel(GetWorld(), GetWorld()->GetGameInstance<UBBGameInstance>()->NewGameData.LevelName);
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("BBMenuMode unable to start game."));
	}
}