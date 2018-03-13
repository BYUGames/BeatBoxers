// copyright 2017 BYU Animation

#include "FretboardWidget.h"

UFretboardWidget::UFretboardWidget(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NoteWidgetClass = UFretboardNoteWidget::StaticClass();
}

void UFretboardWidget::OnNewNote(int NoteID, FNoteData NoteData)
{
	UFretboardNoteWidget *NoteWidget = CreateWidget<UFretboardNoteWidget>(GetWorld(), NoteWidgetClass);
	NoteWidget->NoteID = NoteID;
	if (MyFretboard != nullptr)
	{
		FNoteWidgetData NoteWidgetData;
		NoteWidgetData.NoteType = NoteData.NoteType;
		NoteWidgetData.Track = NoteData.Track;
		NoteWidgetData.Percent = MyFretboard->GetNotePercent(NoteData);
		NoteWidget->NoteWidgetData = NoteWidgetData;
	}
	Notes.Add(NoteID, NoteWidget);
	K2_OnNewNote(NoteWidget);
}

void UFretboardWidget::OnNoteEnd(int NoteID, float Accuracy)
{
	UFretboardNoteWidget **pNoteWidget = Notes.Find(NoteID);
	if (pNoteWidget != nullptr)
	{
		UFretboardNoteWidget *NoteWidget = *pNoteWidget;
		if (NoteWidget != nullptr)
		{
			NoteWidget->OnNoteEnded(Accuracy);
		}
		K2_OnNoteEnded(NoteWidget, Accuracy);
		Notes.Remove(NoteID);
		NoteWidget->OnNoteEnded(Accuracy);
	}
}

void UFretboardWidget::OnNotesCleared()
{
	K2_OnNotesCleared();
	for (TPair<int, UFretboardNoteWidget*> NotePair : Notes)
	{
		if (NotePair.Value != nullptr)
		{
			NotePair.Value->OnNoteCleared();
		}
	}
	Notes.Reset();
}

void UFretboardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MyFretboard != nullptr)
	{
		const TMap<int, FNoteData>& FretboardNotes = MyFretboard->GetAllNotes();
		for (TPair<int, FNoteData> NotePair : FretboardNotes)
		{
			UFretboardNoteWidget **pNoteWidget = Notes.Find(NotePair.Key);
			if (pNoteWidget != nullptr)
			{
				UFretboardNoteWidget *NoteWidget = *pNoteWidget;
				if (NoteWidget != nullptr)
				{
					FNoteWidgetData NoteWidgetData;
					NoteWidgetData.NoteType = NotePair.Value.NoteType;
					NoteWidgetData.Track = NotePair.Value.Track;
					NoteWidgetData.Percent = MyFretboard->GetNotePercent(NotePair.Value);
					NoteWidget->NoteWidgetData = NoteWidgetData;
					K2_OnNoteUpdated(NoteWidget);
				}
			}
		}
	}
}

int UFretboardWidget::BindToFretboard(UObject* Fretboard)
{
	if (Fretboard == nullptr)
	{
		UE_LOG(LogBBUI, Error, TEXT("%s given nullptr as fretboard to bind to."), *GetNameSafe(this));
		return 1;
	}
	else
	{
		OnNotesCleared();
		MyFretboard = Cast<IFretboard>(Fretboard);
		if (MyFretboard == nullptr)
		{
			UE_LOG(LogBBUI, Error, TEXT("%s given %s as fretboard, but it doesn't implement IFretboard interface."), *GetNameSafe(this), *GetNameSafe(Fretboard));
			return 2;
		}
		else
		{
			UE_LOG(LogFretboard, Verbose, TEXT("%s bound to fretboard %s."), *GetNameSafe(this), *GetNameSafe(Fretboard));
			MyFretboard->GetOnNewNoteEvent().AddDynamic(this, &UFretboardWidget::OnNewNote);
			MyFretboard->GetOnNoteEndEvent().AddDynamic(this, &UFretboardWidget::OnNoteEnd);
			MyFretboard->GetOnNotesClearedEvent().AddDynamic(this, &UFretboardWidget::OnNotesCleared);

			const TMap<int, FNoteData>& FretboardNotes = MyFretboard->GetAllNotes();
			for (TPair<int, FNoteData> NotePair : FretboardNotes)
			{
				OnNewNote(NotePair.Key, NotePair.Value);
			}
		}
	}
	return 0;
}

void UFretboardWidget::UnbindFromFretboard()
{
	OnNotesCleared();
	if (MyFretboard != nullptr)
	{
		MyFretboard->GetOnNewNoteEvent().RemoveDynamic(this, &UFretboardWidget::OnNewNote);
		MyFretboard->GetOnNoteEndEvent().RemoveDynamic(this, &UFretboardWidget::OnNoteEnd);
		MyFretboard->GetOnNotesClearedEvent().RemoveDynamic(this, &UFretboardWidget::OnNotesCleared);
		MyFretboard = nullptr;
	}
}