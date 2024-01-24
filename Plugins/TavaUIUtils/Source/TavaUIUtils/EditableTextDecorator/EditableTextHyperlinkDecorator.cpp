// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableTextHyperlinkDecorator.h"

#include "HyperlinkStyleData.h"
#include "WrapWidgetDecorator.h"
#include "Components/RichTextBlock.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/RichEditableText.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Text/SRichTextBlock.h"



TSharedPtr<ITextDecorator> UEditableTextHyperlinkDecorator::CreateDecorator(URichEditableText* InOwner)
{
	EditableOwner = MakeWeakObjectPtr(InOwner);

	//single line text use default decorator (\u0085 in custom has issue with single line editing)
	return SRichTextBlock::WidgetDecorator( TEXT("a"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

TSharedPtr<ITextDecorator> UEditableTextHyperlinkDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	RichTextOwner = MakeWeakObjectPtr(InOwner);

	//multiline text, use custom decorator with correct wrap multiple decorators
	return FWrapWidgetDecorator::Create( TEXT("a"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

FSlateWidgetRun::FWidgetRunInfo UEditableTextHyperlinkDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const ISlateStyle* Style)
{
	const FText Content = RunInfo.MetaData.Contains("Content") ? FText::FromString(RunInfo.MetaData["Content"]) : RunInfo.Content;
	const FString StyleName = RunInfo.MetaData.Contains("Style") ? RunInfo.MetaData["Style"] : TEXT("Link.Default");

	//const FHyperlinkStyle& HyperlinkStyle = Style->HasWidgetStyle<FHyperlinkStyle>(FName(*StyleName))
	//	? Style->GetWidgetStyle<FHyperlinkStyle>(FName(*StyleName))
	//	: FHyperlinkStyle::GetDefault();

	const FHyperlinkStyle& HyperlinkStyle = HyperlinkDecoratorData && HyperlinkDecoratorData->Styles.Contains(FName(*StyleName))
		? HyperlinkDecoratorData->Styles[FName(*StyleName)]
		: FHyperlinkStyle::GetDefault();

	FTextBlockStyle TextBlockStyle(HyperlinkStyle.TextStyle);
	TextBlockStyle.SetFontSize(GetDefaultFont().Size);

	//check text color overrides
	if (RunInfo.MetaData.Contains("TextColor"))
	{
		FLinearColor TextColor;
		TextColor.InitFromString(RunInfo.MetaData["TextColor"]);
		TextBlockStyle.ColorAndOpacity = FSlateColor(TextColor);
	}

	//create widget
	TSharedRef<SHyperlink> WidgetRef = SNew( SHyperlink )
		.Text(Content)
		.Style(&HyperlinkStyle)
		.TextStyle(&TextBlockStyle)
		.OnNavigate(FSimpleDelegate::CreateLambda([=]()
			{
				HandleClick(RunInfo.MetaData);
			}))
		;
	
	//get baseline 
	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const int16 Baseline = FontMeasure->GetBaseline(TextBlockStyle.Font);		

	return FSlateWidgetRun::FWidgetRunInfo( WidgetRef, Baseline );
}

void UEditableTextHyperlinkDecorator::HandleClick(const TMap<FString, FString>& Metadata) const
{
	UE_LOG(LogTemp, Error, TEXT("UEditableTextHyperlinkDecorator::HandleHyperlinkClick"))
	for (auto [Key, Value] : Metadata)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : %s"), *Key, *Value)
	}
	UE_LOG(LogTemp, Error, TEXT("-----"))
	
	//OnHyperlinkClick.Broadcast(FHyperlinkMetadata(Metadata));
}
