#pragma once
#include "RichEditableTextDecorator.h"
#include "Framework/Text/SlateWidgetRun.h"
#include "Widgets/Layout/SScaleBox.h"
#include "EditableTextImageDecorator.generated.h"

class UDataTable;

/**  Create image
 *	Format:
 *		<img id="SmileImage"/>
 *		<img id="SmileImage" width="64" height="desired" stretch="ScaleToFitX"/>
 *		@id Name of image in ImageDecoratorData
 *		@width Width int on desired
 *		@height Height int or desired
 *		@stretch Value Name from EStretch
 */
UCLASS(Abstract)
class TAVAUIUTILS_API UEditableTextImageDecorator : public URichEditableTextDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichEditableText* Owner) override;
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	FSlateWidgetRun::FWidgetRunInfo CreateDecoratorWidget(const FTextRunInfo& TextRunInfo, const ISlateStyle* SlateStyle);

	UPROPERTY(EditAnywhere, Category = "Decorator", meta = (RequiredAssetDataTags = "RowStructure=/Script/UMG.RichImageRow"))
	TObjectPtr<UDataTable> ImageDecoratorData = nullptr;

	void GetImageParameters(const FTextRunInfo& RunInfo, FSlateBrush*& OutBrush, TOptional<int32>& OutWidth, TOptional<int32>& OutHeight, EStretch::Type& OutStretch);
};
