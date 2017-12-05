// copyright 2017 BYU Animation

#include "FretboardNoteWidget.h"

UFretboardNoteWidget::UFretboardNoteWidget(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UFretboardNoteWidget::OnNoteEnded_Implementation(float Accuracy)
{
	RemoveFromParent();
}

void UFretboardNoteWidget::OnNoteCleared_Implementation()
{
	RemoveFromParent();
}