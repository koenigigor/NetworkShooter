// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSendMessageWindow.h"

#include "Widgets/RichEditableText.h"


void UChatSendMessageWindow::NativeConstruct()
{
	Super::NativeConstruct();

	if (InputText)
	{
		InputText->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChanged);
	}
	
}

void UChatSendMessageWindow::AppendText(const FText& Text)
{
	check(InputText);
	SetText(FText::Format(FTextFormat::FromString("{0}{1}"), InputText->GetText(), Text));
}

void UChatSendMessageWindow::SetText(const FText& Text)
{
	check(InputText);
	InputText->SetText(Text);

#if ENGINE_MINOR_VERSION >= 2
	// In UE 5.2 by commit 59be5e6 OnTextChanged no more triggers on direct set text
	OnTextChanged(InputText->GetText());
#endif
}

void UChatSendMessageWindow::OnTextChanged(const FText& Text)
{
	//parcse entered text for tags, and replace it to decorators
	// * for future refactor, iterate string from end, for replace strings copy, and second loop
	
	FString SourceString = Text.ToString();

	struct FReplaceResult
	{
		int32 StartIndex;
		int32 EndIndex;
		FString Name;
		FString Replacer;
	};
	
	// Find all tags who can be potentially replaced by decorators (:My_Tag:)
	TArray<FReplaceResult> ReplaceResults;
	for (int32 i = 0; i != SourceString.Len()-1; i++)
	{
		const int32 DecoratorOpenIndex = SourceString.Find("<", ESearchCase::CaseSensitive, ESearchDir::FromStart, i);
		const int32 DecoratorCloseIndex = SourceString.Find("/>", ESearchCase::CaseSensitive, ESearchDir::FromStart, i+1);
		const int32 TagOpenIndex = SourceString.Find(":", ESearchCase::CaseSensitive, ESearchDir::FromStart, i);
		const int32 TagCloseIndex = SourceString.Find(":", ESearchCase::CaseSensitive, ESearchDir::FromStart, TagOpenIndex + 1);

		//no more tags exist, end loop
		if (TagOpenIndex == INDEX_NONE || TagCloseIndex == INDEX_NONE || TagOpenIndex == TagCloseIndex) break;
		
		//we found decorator first, skip it
		if (DecoratorOpenIndex != INDEX_NONE && DecoratorCloseIndex != INDEX_NONE && TagCloseIndex > DecoratorOpenIndex)
		{
			i = DecoratorCloseIndex;
			continue;
		}

		const FString TagName = SourceString.Mid(TagOpenIndex + 1, TagCloseIndex - TagOpenIndex - 1);
		const FString Replacer = GetReplacerForTag(TagName);
		
		//can replace this tag or not
		if (!Replacer.IsEmpty())
		{
			ReplaceResults.Add({TagOpenIndex, TagCloseIndex, TagName, Replacer});
            i = TagCloseIndex;
		}
		else
		{
			i = TagOpenIndex;
		}
	}

	//replace all found tags from end
	for (int32 i = ReplaceResults.Num()-1; i>=0; i--)
	{
		auto& TagToReplace = ReplaceResults[i];

		SourceString.RemoveAt(TagToReplace.StartIndex, TagToReplace.Name.Len()+2);
		SourceString.InsertAt(TagToReplace.StartIndex, TagToReplace.Replacer);
	}

	//push modified string back to text block
	if (ReplaceResults.Num() != 0)
	{
		InputText->SetText(FText::FromString(SourceString));
	}
}


FString UChatSendMessageWindow::GetReplacerForTag(const FString& Tag)
{
	if (ReplaceTagMap.Contains(Tag))
		return ReplaceTagMap[Tag];
	
	return FString();
}

