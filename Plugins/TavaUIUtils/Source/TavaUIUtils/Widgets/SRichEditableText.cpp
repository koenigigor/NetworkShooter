#include "SRichEditableText.h"

#include "Framework/Text/PlainTextLayoutMarshaller.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Types/TrackedMetaData.h"
#include "Widgets/Text/SlateEditableTextLayout.h"


void SRichEditableText::Construct(const FArguments& InArgs)
{
	bIsReadOnly = InArgs._IsReadOnly;
	bIsPassword = InArgs._IsPassword;

	bIsCaretMovedWhenGainFocus = InArgs._IsCaretMovedWhenGainFocus;
	bSelectAllTextWhenFocused = InArgs._SelectAllTextWhenFocused;
	bRevertTextOnEscape = InArgs._RevertTextOnEscape;
	bClearKeyboardFocusOnCommit = InArgs._ClearKeyboardFocusOnCommit;
	bAllowContextMenu = InArgs._AllowContextMenu;
	OnContextMenuOpening = InArgs._OnContextMenuOpening;
	OnIsTypedCharValid = InArgs._OnIsTypedCharValid;
	OnTextChangedCallback = InArgs._OnTextChanged;
	OnTextCommittedCallback = InArgs._OnTextCommitted;
	MinDesiredWidth = InArgs._MinDesiredWidth;
	bSelectAllTextOnCommit = InArgs._SelectAllTextOnCommit;
	bSelectWordOnMouseDoubleClick = InArgs._SelectWordOnMouseDoubleClick;
	VirtualKeyboardType = InArgs._VirtualKeyboardType;
	VirtualKeyboardOptions = InArgs._VirtualKeyboardOptions;
	VirtualKeyboardTrigger = InArgs._VirtualKeyboardTrigger;
	VirtualKeyboardDismissAction = InArgs._VirtualKeyboardDismissAction;
	OnKeyCharHandler = InArgs._OnKeyCharHandler;
	OnKeyDownHandler = InArgs._OnKeyDownHandler;

	Font = InArgs._Font;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	BackgroundImageSelected = InArgs._BackgroundImageSelected;

	// We use the given style when creating the text layout as it may not be safe to call the override delegates until we've finished being constructed
	// The first call to SynchronizeTextStyle will apply the correct overrides, and that will happen before the first paint
	check(InArgs._Style);
	FTextBlockStyle TextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	TextStyle.Font = InArgs._Style->Font;
	TextStyle.ColorAndOpacity = InArgs._Style->ColorAndOpacity;
	TextStyle.HighlightShape = InArgs._Style->BackgroundImageSelected;

	PlainTextMarshaller = FPlainTextLayoutMarshaller::Create();
    PlainTextMarshaller->SetIsPassword(bIsPassword);

	//Rich text marshaller
	{
		RichTextMarshaller = InArgs._RichTextLayoutMarshaller;
	}

	

	// We use a separate marshaller for the hint text, as that should never be displayed as a password
	TSharedRef<FPlainTextLayoutMarshaller> HintTextMarshaller = FPlainTextLayoutMarshaller::Create();

	EditableTextLayout = MakeUnique<FSlateEditableTextLayout>(*this, InArgs._Text, TextStyle, InArgs._TextShapingMethod, InArgs._TextFlowDirection, FCreateSlateTextLayout(), RichTextMarshaller.ToSharedRef(), HintTextMarshaller);
	EditableTextLayout->SetHintText(InArgs._HintText);
	EditableTextLayout->SetSearchText(InArgs._SearchText);
	EditableTextLayout->SetCursorBrush(InArgs._CaretImage.IsSet() ? InArgs._CaretImage : &InArgs._Style->CaretImage);
	EditableTextLayout->SetCompositionBrush(InArgs._BackgroundImageComposing.IsSet() ? InArgs._BackgroundImageComposing : &InArgs._Style->BackgroundImageComposing);
	EditableTextLayout->SetDebugSourceInfo(TAttribute<FString>::Create(TAttribute<FString>::FGetter::CreateLambda([this]{ return FReflectionMetaData::GetWidgetDebugInfo(this); })));
	EditableTextLayout->SetJustification(InArgs._Justification);
	EditableTextLayout->SetOverflowPolicy(InArgs._OverflowPolicy);

	// build context menu extender
	MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuExtension("EditText", EExtensionHook::Before, TSharedPtr<FUICommandList>(), InArgs._ContextMenuExtender);

	AddMetadata(MakeShared<FTrackedMetaData>(this, FName(TEXT("EditableText"))));
}

const FTextBlockStyle& SRichEditableText::GetTextStyle() const
{
	return EditableTextLayout->GetTextStyle();
}


