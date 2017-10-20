// copyright 2017 BYU Animation

#include "NoteKeeper.h"

UNoteKeeper::UNoteKeeper(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NextID = 0;
	Tolerance = 0.1f;
}

float UNoteKeeper::HitNoteType(ENoteType Type)
{
	float retAccuracy = -1.f;
	TArray<int> NotesHitIndexes;
	for (TPair<int, FNoteData> NotePair : Notes)
	{
		if (NotePair.Value.NoteType == Type)
		{
			float Accuracy = GetNoteTimingAccuracy(NotePair.Value);
			if (Accuracy <= Tolerance)
			{
				NotesHitIndexes.Add(NotePair.Key);
				if (retAccuracy < 0 || Accuracy < retAccuracy)
				{
					retAccuracy = Accuracy;
				}
			}
		}
	}
	for (int Index : NotesHitIndexes)
	{
		EndNote(Index);
	}
	return retAccuracy;
}

void UNoteKeeper::AddNote(FNoteData NewNote)
{
	int NoteID = NextID++;
	Notes.Add(NoteID, NewNote);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UNoteKeeper::EndNote, NoteID);
	TimerManager->SetTimer(Notes[NoteID].TimerHandle, TimerDelegate, NewNote.InitialDuration, false);
	if (NewNoteKeeperNoteEvent.IsBound())
	{
		NewNoteKeeperNoteEvent.Broadcast(NoteID, Notes[NoteID]);
	}
}

float UNoteKeeper::GetNoteTimingAccuracy(FNoteData& NoteData) const
{
	if (TimerManager->IsTimerActive(NoteData.TimerHandle))
	{
		float Accuracy = TimerManager->GetTimerRemaining(NoteData.TimerHandle) / NoteData.InitialDuration;
	}
	else
	{
		return -1.f;
	}
}

void UNoteKeeper::EndNote(int Index)
{
	if (NoteEndEvent.IsBound())
	{
		float Accuracy = GetNoteTimingAccuracy(Notes[Index]);
		NoteEndEvent.Broadcast(Index, Accuracy <= Tolerance ? Accuracy : -1.f);
	}
	TimerManager->ClearTimer(Notes[Index].TimerHandle);
	Notes.Remove(Index);
}