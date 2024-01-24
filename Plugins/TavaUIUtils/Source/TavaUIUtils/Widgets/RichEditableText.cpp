#include "RichEditableText.h"

#include "RichEditableTextDecorator.h"
#include "SRichEditableText.h"
#include "Framework/SlateDelegates.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"

TSharedRef<SWidget> URichEditableText::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS

	TArray< TSharedRef< ITextDecorator > > CreatedDecorators;
	CreateDecorators(CreatedDecorators);

	TSharedRef<FRichTextLayoutMarshaller> Marshaller = FRichTextLayoutMarshaller::Create(CreatedDecorators, &FCoreStyle::Get());
	
	MyEditableText = SNew(SRichEditableText)
		.Style(&WidgetStyle)
		.MinDesiredWidth(MinimumDesiredWidth)
		.IsCaretMovedWhenGainFocus(IsCaretMovedWhenGainFocus)
		.SelectAllTextWhenFocused(SelectAllTextWhenFocused)
		.RevertTextOnEscape(RevertTextOnEscape)
		.ClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit)
		.SelectAllTextOnCommit(SelectAllTextOnCommit)
		.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnTextChanged))
		.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, HandleOnTextCommitted))
		.VirtualKeyboardType(EVirtualKeyboardType::AsKeyboardType(KeyboardType.GetValue()))
		.VirtualKeyboardOptions(VirtualKeyboardOptions)
		.VirtualKeyboardTrigger(VirtualKeyboardTrigger)
		.VirtualKeyboardDismissAction(VirtualKeyboardDismissAction)
		.Justification(Justification)
		.OverflowPolicy(OverflowPolicy)
		.RichTextLayoutMarshaller(Marshaller)
		;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	
	return MyEditableText.ToSharedRef();
}

void URichEditableText::CreateDecorators(TArray<TSharedRef<ITextDecorator>>& OutDecorators)
{
	//URichTextBlock::UpdateStyleData
	{
		if (IsDesignTime())
		{
			InstanceDecorators.Reset();
		}

		{
			for (const auto& DecoratorClass : DecoratorClasses)
			{
				URichEditableTextDecorator* Decorator = NewObject<URichEditableTextDecorator>(this, DecoratorClass);
				InstanceDecorators.Add(Decorator);
			}
		}
	}
	
	for (auto Decorator : InstanceDecorators)
	{
		if (Decorator)
		{
			TSharedPtr<ITextDecorator> TextDecorator = Decorator->CreateDecorator(this);
			if (TextDecorator.IsValid())
			{
				OutDecorators.Add(TextDecorator.ToSharedRef());
			}
		}
	}
}

void URichEditableText::SetFontSize(int32 InSize)
{
	auto Font = GetFont();
	Font.Size = InSize;
	
	WidgetStyle.SetFont(Font);
	if (MyEditableText.IsValid())
	{
		MyEditableText->SetTextStyle(WidgetStyle);
	}
}

const FTextBlockStyle& URichEditableText::GetTextStyle() const
{
	if (MyEditableText.IsValid())
	{
		SRichEditableText* MyRichEditableText = (SRichEditableText*)MyEditableText.Get();
		return MyRichEditableText->GetTextStyle();
	}

	static FTextBlockStyle EmptyTextBlockStyle;
	return EmptyTextBlockStyle;
}
