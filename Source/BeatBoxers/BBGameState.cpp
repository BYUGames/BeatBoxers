// copyright 2017 BYU Animation

#include "BBGameState.h"

ABBGameState::ABBGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Scores = TMap<AController*, float>();
}

float ABBGameState::GetScore(AController* Controller)
{
	return Scores.FindOrAdd(Controller);
}

float ABBGameState::AddScore(AController* Controller, float Amount)
{
	float OldScore = Scores.FindOrAdd(Controller);
	return Scores.Add(Controller, OldScore + Amount);
}

float ABBGameState::SetScore(AController* Controller, float Value)
{
	return Scores.Add(Controller, Value);
}

void ABBGameState::ResetScores()
{
	Scores.Reset();
}