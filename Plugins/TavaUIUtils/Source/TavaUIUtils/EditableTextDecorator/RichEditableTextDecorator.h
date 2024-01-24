// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "RichEditableTextDecorator.generated.h"

class ITextDecorator;
class URichEditableText;


/** @brief After long search with copy decortor text on clipboard (like in discord) select this solution:
 *	use decorator with tag "ClipboardOverride" and value with string whe must be copied,
 *	and update function GetRangeAsTextFromLine in TextLayout.cpp in ue source build
 *	create and use own text layout is ineffective */

/* (from UE5.1)
void GetRangeAsTextFromLine(FString& DisplayText, const FTextLayout::FLineModel& LineModel, const FTextRange& Range)
{
	for (int32 RunIndex = 0; RunIndex < LineModel.Runs.Num(); RunIndex++)
	{
		const FTextLayout::FRunModel& Run = LineModel.Runs[RunIndex];
		const FTextRange& RunRange = Run.GetTextRange();

		const FTextRange IntersectRange = RunRange.Intersect(Range);

		if (!IntersectRange.IsEmpty())
		{
			if (Run.GetRun()->GetRunInfo().MetaData.Contains("ClipboardOverride"))
			{
				DisplayText.Append(Run.GetRun()->GetRunInfo().MetaData["ClipboardOverride"]);
			}
			else
			{
				Run.AppendTextTo(DisplayText, IntersectRange);
			}
		}
		else if (RunRange.BeginIndex > Range.EndIndex)
		{
			//We're past the selection range so we can stop
			break;
		}
	}
}
*/


/**  Base class for combined decorator for URichEditableText and URichTextBlock */
UCLASS(Abstract)
class TAVAUIUTILS_API URichEditableTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichEditableText* Owner);
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual UWorld* GetWorld() const override;
	
protected:
	const FSlateFontInfo& GetDefaultFont() const;
	const FTextBlockStyle& GetTextBlockStyle() const;

	TWeakObjectPtr<URichEditableText> EditableOwner = nullptr;
	TWeakObjectPtr<URichTextBlock> RichTextOwner = nullptr;
};


