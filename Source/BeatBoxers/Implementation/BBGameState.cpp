// copyright 2017 BYU Animation

#include "BBGameState.h"


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

void ABBGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); // Call parent class tick function  
	FVector p1Loc = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn()->GetActorLocation();
	FVector p2Loc = UGameplayStatics::GetPlayerController(GetWorld(), 1)->GetPawn()->GetActorLocation();
	FVector camLoc = MainCamera->GetActorLocation();
	MainCamera->SetActorLocation(FVector((p1Loc.X + p2Loc.X) / 2, camLoc.Y, (p1Loc.Z + p2Loc.Z) / 2 + 100));
}