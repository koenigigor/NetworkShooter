// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableTextRMBDecorator.h"

#include "WrapWidgetDecorator.h"
#include "Widgets/SButtonRMB.h"
#include "EditableTextDecorator/HyperlinkStyleData.h"
#include "Components/RichTextBlock.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/RichEditableText.h"
#include "Widgets/Text/SRichTextBlock.h"


UEditableTextRMBDecorator::UEditableTextRMBDecorator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultButtonStyle.Normal.TintColor = FSlateColor(FLinearColor::Transparent);
	DefaultButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor::Transparent);
	DefaultButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor::Transparent);
}

TSharedPtr<ITextDecorator> UEditableTextRMBDecorator::CreateDecorator(URichTextBlock* Owner)
{
	RichTextOwner = MakeWeakObjectPtr(Owner);

	//multiline text, use custom decorator with correct wrap multiple decorators
	return FWrapWidgetDecorator::Create( TEXT("LinkRMB"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

TSharedPtr<ITextDecorator> UEditableTextRMBDecorator::CreateDecorator(URichEditableText* Owner)
{
	EditableOwner = MakeWeakObjectPtr(Owner);

	//single line text use default decorator (\u0085 in custom has issue with single line editing)
	return SRichTextBlock::WidgetDecorator( TEXT("LinkRMB"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

FSlateWidgetRun::FWidgetRunInfo UEditableTextRMBDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const ISlateStyle* SlateStyle)
{
	const FText Content = RunInfo.MetaData.Contains("Content") ? FText::FromString(RunInfo.MetaData["Content"]) : RunInfo.Content;
	const FString StyleName = RunInfo.MetaData.Contains("Style") ? RunInfo.MetaData["Style"] : TEXT("Link.Default");

	const bool bStyleExistInDecoratorData = HyperlinkDecoratorData && HyperlinkDecoratorData->Styles.Contains(FName(*StyleName));
	const FHyperlinkStyle& HyperlinkStyle = bStyleExistInDecoratorData
		? HyperlinkDecoratorData->Styles[FName(*StyleName)]
		: FHyperlinkStyle::GetDefault();
	
	FTextBlockStyle TextStyle = bStyleExistInDecoratorData ? HyperlinkStyle.TextStyle : GetTextBlockStyle();
	FButtonStyle ButtonStyle = bStyleExistInDecoratorData ? HyperlinkStyle.UnderlineStyle : DefaultButtonStyle;
	
	TextStyle.SetFontSize(GetTextBlockStyle().Font.Size);

	if (RunInfo.MetaData.Contains("TextColor"))
	{
		FLinearColor TextColor;
		TextColor.InitFromString(RunInfo.MetaData["TextColor"]);
		TextStyle.ColorAndOpacity = FSlateColor(TextColor);
	}

	if (RunInfo.MetaData.Contains("UseParentColor"))
	{
		const auto& ParentColor = GetTextBlockStyle().ColorAndOpacity;
		TextStyle.ColorAndOpacity = ParentColor;
		ButtonStyle.Normal.TintColor = ParentColor;
		ButtonStyle.Hovered.TintColor = ParentColor;
		ButtonStyle.Pressed.TintColor = ParentColor;
	}

	ButtonStyles.Add(ButtonStyle);
	
	auto WidgetRef = SNew(SButtonRMB)
		.OnClicked(FOnClicked::CreateLambda([=]()->FReply
		{
			OnClickLeft(RunInfo.MetaData);
			return FReply::Handled();
		}))		
		.OnClickedRight(FOnClicked::CreateLambda([=]()->FReply
		{
			OnClickRight(RunInfo.MetaData);
			return FReply::Handled();
		}))
		.ClickMethod(EButtonClickMethod::PreciseClick)
		.IsFocusable(false)
		.Cursor(EMouseCursor::Hand)
		.Text(RunInfo.MetaData.Contains("Content")? FText::FromString(RunInfo.MetaData["Content"]) : RunInfo.Content)
		.TextStyle(&TextStyle)
		.ContentPadding(FMargin(0.f))
		.ButtonStyle(&ButtonStyles.Last());
	
	//get baseline 
	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const int16 Baseline = FontMeasure->GetBaseline(TextStyle.Font);		

	return FSlateWidgetRun::FWidgetRunInfo( WidgetRef, Baseline );	
}

