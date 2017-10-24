// copyright 2017 BYU Animation

#include "BasicFretboard.h"
#include "BBFunctionLibrary.h"

UBasicFretboard::UBasicFretboard(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NextID = 0;
	Tolerance = 0.1f;
}

FFretboardInputResult UBasicFretboard::ReceiveInputToken(EInputToken InputToken)
{
	ENoteType HitType = UBBFunctionLibrary::GetNoteFromInput(InputToken);
	FFretboardInputResult Result;
	Result.Accuracy = -1.f;
	TArray<int> NotesHitIndexes;
	for (TPair<int, FNoteData> NotePair : Notes)
	{
		if (NotePair.Value.NoteType == HitType)
		{
			float Accuracy = GetNoteTimingAccuracy(NotePair.Value);
			if (Accuracy >= 0.f)
			{
				NotesHitIndexes.Add(NotePair.Key);
				if (Result.Accuracy < 0 || Accuracy > Result.Accuracy)
				{
					Result.Accuracy = Accuracy;
				}
			}
		}
	}
	for (int Index : NotesHitIndexes)
	{
		EndNote(Index);
	}
	return Result;
}

void UBasicFretboard::AddNote(FFeedNoteData FeedNote)
{
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
		TimerDelegate.BindUObject(this, &UBasicFretboard::EndNote, NoteID);
		TimerManager->SetTimer(NewNote.TimerHandle, TimerDelegate, FeedNote.DurationRemaining, false);
		Notes.Add(NoteID, NewNote);
		if (NewNoteEvent.IsBound())
		{
			NewNoteEvent.Broadcast(NoteID, NewNote);
		}
	}
}

float UBasicFretboard::GetNotePercent(FNoteData NoteData)
{
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		if (TimerManager->IsTimerActive(NoteData.TimerHandle))
		{
			return TimerManager->GetTimerElapsed(NoteData.TimerHandle) / NoteData.InitialDuration;
		}
	}
	return -1.f;
}

void UBasicFretboard::PauseBoard()
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

void UBasicFretboard::ResumeBoard()
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

void UBasicFretboard::ClearBoard()
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
		if (NotesClearedEvent.IsBound())
		{
			NotesClearedEvent.Broadcast();
		}
	}
}

float UBasicFretboard::GetNoteTimingAccuracy(FNoteData NoteData)
{
	if (TimerManager == nullptr)
	{
		UE_LOG(LogBeatBoxersCriticalErrors, Error, TEXT("%s used without a valid TimerManager reference."), *GetNameSafe(this));
	}
	else
	{
		if (TimerManager->IsTimerActive(NoteData.TimerHandle))
		{
			float Percent = GetNotePercent(NoteData);
			if (Percent >= Tolerance)
			{
				return 1.f - ((1.f - Percent) / (1.f - Tolerance));
			}
		}
	}
	return -1.f;
}

void UBasicFretboard::EndNote(int Index)
{
	if (NoteEndEvent.IsBound())
	{
		NoteEndEvent.Broadcast(Index, GetNoteTimingAccuracy(Notes[Index]));
	}
	TimerManager->ClearTimer(Notes[Index].TimerHandle);
	Notes.Remove(Index);
}

void UBasicFretboard::Listen(TWeakObjectPtr<UObject> FretboardFeed)
{
	StopListening();
	if (FretboardFeed.IsValid())
	{
		MyFretboardFeed = Cast<IFretboardFeed>(FretboardFeed.Get());
		if (MyFretboardFeed == nullptr)
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("%s given %s as a FretboardFeed, but it doesn't implement IFretboardFeed."), *GetNameSafe(this), *GetNameSafe(FretboardFeed.Get()));
		}
		else
		{
			MyFretboardFeed->GetOnNewFeedNoteEvent().AddDynamic(this, &UBasicFretboard::AddNote);
			TArray<FFeedNoteData> NewNotes = MyFretboardFeed->GetExistingNotes();
			for (FFeedNoteData NewNote : NewNotes)
			{
				AddNote(NewNote);
			}
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Warning, TEXT("%s given invalid object to use as feed."), *GetNameSafe(this));
	}
}

void UBasicFretboard::StopListening()
{
	if (MyFretboardFeed != nullptr)
	{
		MyFretboardFeed->GetOnNewFeedNoteEvent().RemoveDynamic(this, &UBasicFretboard::AddNote);
	}
	ClearBoard();
}
