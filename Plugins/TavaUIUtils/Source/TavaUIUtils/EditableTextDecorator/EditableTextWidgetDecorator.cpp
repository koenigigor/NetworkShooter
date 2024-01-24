// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableTextWidgetDecorator.h"

#include "WrapWidgetDecorator.h"
#include "Components/RichTextBlock.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/RichEditableText.h"
#include "Widgets/Text/SRichTextBlock.h"


TSharedPtr<ITextDecorator> UEditableTextWidgetDecorator::CreateDecorator(URichTextBlock* Owner)
{
	RichTextOwner = MakeWeakObjectPtr(Owner);

	//multiline text, use custom decorator with correct wrap multiple decorators
	return FWrapWidgetDecorator::Create( TEXT("Widget"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

TSharedPtr<ITextDecorator> UEditableTextWidgetDecorator::CreateDecorator(URichEditableText* Owner)
{
	EditableOwner = MakeWeakObjectPtr(Owner);

	//single line text use default decorator (\u0085 in custom has issue with single line editing)
	return SRichTextBlock::WidgetDecorator( TEXT("Widget"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

FSlateWidgetRun::FWidgetRunInfo UEditableTextWidgetDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const ISlateStyle* SlateStyle)
{
	const FText Content = RunInfo.MetaData.Contains("Content") ? FText::FromString(RunInfo.MetaData["Content"]) : RunInfo.Content;

	UWidget* Widget = CreateWidget(GetTextBlockStyle(), Content, RunInfo.MetaData);

	TSharedRef<SWidget> WidgetRef = Widget
		? Widget->TakeWidget()
		: SNew(STextBlock)
			.Visibility(EVisibility::HitTestInvisible)
			.Text( Content )
			.TextStyle( &GetTextBlockStyle());

	//get baseline 
	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const int16 Baseline = FontMeasure->GetBaseline(GetTextBlockStyle().Font);		

	return FSlateWidgetRun::FWidgetRunInfo( WidgetRef, Baseline );
}

UWidget* UEditableTextWidgetDecorator::CreateWidget_Implementation(const FTextBlockStyle& TextStyle, const FText& Content, const TMap<FString, FString>& Metadata)
{	
	return nullptr;
}
