// copyright 2017 BYU Animation

#include "BasicFretboardFeed.h"

UBasicFretboardFeed::UBasicFretboardFeed(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NoteDuration = 5.f;
	NextID = 0;
}

TArray<FFeedNoteData> UBasicFretboardFeed::GetExistingNotes()
{
	TArray<FFeedNoteData> FeedNotes;
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		FFeedNoteData FeedNote;
		for(TPair<int, FNoteData> NotePair : Notes)
		{
			FeedNote.NoteType = NotePair.Value.NoteType;
			FeedNote.Track = NotePair.Value.Track;
			FeedNote.InitialDuration = NoteDuration;
			if (TimerManager->IsTimerActive(NotePair.Value.TimerHandle))
			{
				FeedNote.DurationRemaining = TimerManager->GetTimerRemaining(NotePair.Value.TimerHandle);
				FeedNotes.Add(FeedNote);
			}
		}
	}
	return FeedNotes;
}

void UBasicFretboardFeed::PauseFeed()
{
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		for (TPair<int, FNoteData> NotePair : Notes)
		{
			if (TimerManager->IsTimerActive(NotePair.Value.TimerHandle))
			{
				TimerManager->PauseTimer(NotePair.Value.TimerHandle);
			}
		}
	}
}

void UBasicFretboardFeed::ResumeFeed()
{
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		for (TPair<int, FNoteData> NotePair : Notes)
		{
			if (TimerManager->IsTimerPaused(NotePair.Value.TimerHandle))
			{
				TimerManager->UnPauseTimer(NotePair.Value.TimerHandle);
			}
		}
	}
}

void UBasicFretboardFeed::ClearFeed()
{
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		for (TPair<int, FNoteData> NotePair : Notes)
		{
			if (TimerManager->TimerExists(NotePair.Value.TimerHandle))
			{
				TimerManager->ClearTimer(NotePair.Value.TimerHandle);
			}
		}
		Notes.Empty();
	}
}

void UBasicFretboardFeed::AddNote(FFeedNoteData FeedNote)
{
	FeedNote.InitialDuration = NoteDuration;
	if (FeedNote.DurationRemaining <= 0)
	{
		FeedNote.DurationRemaining = NoteDuration;
	}
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		int NoteID = NextID++;
		FNoteData NewNote;
		NewNote.NoteType = FeedNote.NoteType;
		NewNote.Track = FeedNote.Track;
		NewNote.InitialDuration = FeedNote.InitialDuration;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UBasicFretboardFeed::EndNote, NoteID);
		TimerManager->SetTimer(NewNote.TimerHandle, TimerDelegate, FeedNote.DurationRemaining, false);
		Notes.Add(NoteID, NewNote);
		if (NewFeedNoteEvent.IsBound())
		{
			NewFeedNoteEvent.Broadcast(FeedNote);
		}
	}
}

void UBasicFretboardFeed::EndNote(int Index)
{
	TimerManager->ClearTimer(Notes[Index].TimerHandle);
	Notes.Remove(Index);
}