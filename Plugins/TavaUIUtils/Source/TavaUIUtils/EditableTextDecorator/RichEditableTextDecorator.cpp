// Fill out your copyright notice in the Description page of Project Settings.


#include "RichEditableTextDecorator.h"

#include "Components/RichTextBlock.h"
#include "Widgets/RichEditableText.h"

TSharedPtr<ITextDecorator> URichEditableTextDecorator::CreateDecorator(URichEditableText* Owner)
{
	EditableOwner = MakeWeakObjectPtr(Owner);
	return TSharedPtr<ITextDecorator>();
}

TSharedPtr<ITextDecorator> URichEditableTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	RichTextOwner = MakeWeakObjectPtr(InOwner);
	return Super::CreateDecorator(InOwner);
}

UWorld* URichEditableTextDecorator::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld) return nullptr; 
	if (GetOuter()) return GetOuter()->GetWorld(); 
	return nullptr;
}

const FSlateFontInfo& URichEditableTextDecorator::GetDefaultFont() const
{
	if (EditableOwner.IsValid())
	{
		return EditableOwner->GetFont();
	}
	if (RichTextOwner.IsValid())
	{
		return RichTextOwner->GetCurrentDefaultTextStyle().Font;
	}

	ensure(0);
	static FSlateFontInfo Default;
	return Default;
}

const FTextBlockStyle& URichEditableTextDecorator::GetTextBlockStyle() const
{
	if (EditableOwner.IsValid())
	{
		return EditableOwner->GetTextStyle();
	}
	if (RichTextOwner.IsValid())
	{
		return RichTextOwner->GetCurrentDefaultTextStyle();
	}

	ensure(0);
	static FTextBlockStyle Default;
	return Default;	
}
