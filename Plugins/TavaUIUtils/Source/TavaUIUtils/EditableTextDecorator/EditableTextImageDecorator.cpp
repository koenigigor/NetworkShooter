#include "EditableTextImageDecorator.h"

#include "WrapWidgetDecorator.h"
#include "Components/RichTextBlock.h"
#include "Components/RichTextBlockImageDecorator.h"
#include "Engine/DataTable.h"
#include "Fonts/FontMeasure.h"
#include "Misc/DefaultValueHelper.h"
#include "Widgets/RichEditableText.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/SRichTextBlock.h"


/** copy of SRichInlineImage from RichTextBlockImageDecorator.cpp */
class SMyRichInlineImage : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyRichInlineImage)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const FSlateBrush* Brush, const FTextBlockStyle& TextStyle, TOptional<int32> Width, TOptional<int32> Height, EStretch::Type Stretch);
};

void SMyRichInlineImage::Construct(const FArguments& InArgs, const FSlateBrush* Brush, const FTextBlockStyle& TextStyle, TOptional<int32> Width,
	TOptional<int32> Height, EStretch::Type Stretch)
{
	check(Brush);

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	float IconHeight = FMath::Min((float)FontMeasure->GetMaxCharacterHeight(TextStyle.Font, 1.0f), Brush->ImageSize.Y);

	if (Height.IsSet())
	{
		IconHeight = Height.GetValue();
	}

	float IconWidth = IconHeight;
	if (Width.IsSet())
	{
		IconWidth = Width.GetValue();
	}

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(IconHeight)
		.WidthOverride(IconWidth)
		[
			SNew(SScaleBox)
			.Stretch(Stretch)
			.StretchDirection(EStretchDirection::DownOnly)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(Brush)
			]
		]
	];
}

//------


TSharedPtr<ITextDecorator> UEditableTextImageDecorator::CreateDecorator(URichEditableText* Owner)
{
	EditableOwner = MakeWeakObjectPtr(Owner);

	//single line text use default decorator (\u0085 in custom has issue with single line editing)
	return SRichTextBlock::WidgetDecorator( TEXT("img"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

TSharedPtr<ITextDecorator> UEditableTextImageDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	RichTextOwner = MakeWeakObjectPtr(InOwner);
	
	//multiline text, use custom decorator with correct wrap multiple decorators
	return FWrapWidgetDecorator::Create( TEXT("img"), FWidgetDecorator::FCreateWidget::CreateUObject(this, &ThisClass::CreateDecoratorWidget));
}

FSlateWidgetRun::FWidgetRunInfo UEditableTextImageDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const ISlateStyle* SlateStyle)
{
	FSlateBrush* Brush = nullptr;
	TOptional<int32> Width;
	TOptional<int32> Height;
	EStretch::Type Stretch = EStretch::ScaleToFit;
	GetImageParameters(RunInfo, Brush, Width, Height, Stretch);

	TSharedRef<SMyRichInlineImage> WidgetRef = SNew(SMyRichInlineImage, Brush, GetTextBlockStyle(), Width, Height, Stretch);

	const FSlateFontInfo Font = GetDefaultFont();
	const float ShadowOffsetY = FMath::Min(0.0f, GetTextBlockStyle().ShadowOffset.Y);
	
	TAttribute<int16> GetBaseline = TAttribute<int16>::CreateLambda([Font, ShadowOffsetY]()
	{	
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		return FontMeasure->GetBaseline(Font) - ShadowOffsetY;
	});

	return FSlateWidgetRun::FWidgetRunInfo( WidgetRef, GetBaseline );
}

void UEditableTextImageDecorator::GetImageParameters(const FTextRunInfo& RunInfo, FSlateBrush*& OutBrush, TOptional<int32>& OutWidth,
	TOptional<int32>& OutHeight, EStretch::Type& OutStretch)
{
	static FSlateBrush DefaultBrush;
	OutBrush = &DefaultBrush;
	
	if (!ensure(ImageDecoratorData)) return;
	
	const FString ContextString;
	const auto ImageRow = ImageDecoratorData->FindRow<FRichImageRow>(FName(*RunInfo.MetaData[TEXT("id")]), ContextString, true);
	if (!ensure(ImageRow)) return;
	
	OutBrush = &ImageRow->Brush;
	if (!ensure(OutBrush)) return;

	if (const FString* WidthString = RunInfo.MetaData.Find(TEXT("width")))
	{
		int32 WidthTemp;
		if (FDefaultValueHelper::ParseInt(*WidthString, WidthTemp))
		{
			OutWidth = WidthTemp;
		}
		else
		{
			if (FCString::Stricmp(GetData(*WidthString), TEXT("desired")) == 0)
			{
				OutWidth = OutBrush->ImageSize.X;
			}
		}
	}

	if (const FString* HeightString = RunInfo.MetaData.Find(TEXT("height")))
	{
		int32 HeightTemp;
		if (FDefaultValueHelper::ParseInt(*HeightString, HeightTemp))
		{
			OutHeight = HeightTemp;
		}
		else
		{
			if (FCString::Stricmp(GetData(*HeightString), TEXT("desired")) == 0)
			{
				OutHeight = OutBrush->ImageSize.Y;
			}
		}
	}

	if (const FString* StretchString = RunInfo.MetaData.Find(TEXT("stretch")))
	{
		const UEnum* StretchEnum = StaticEnum<EStretch::Type>();
		int64 StretchValue = StretchEnum->GetValueByNameString(*StretchString);
		if (StretchValue != INDEX_NONE)
		{
			OutStretch = static_cast<EStretch::Type>(StretchValue);
		}
	}	
}
