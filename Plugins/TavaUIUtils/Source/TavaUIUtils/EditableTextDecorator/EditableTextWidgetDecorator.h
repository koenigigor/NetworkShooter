// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RichEditableTextDecorator.h"
#include "Framework/Text/SlateWidgetRun.h"
#include "EditableTextWidgetDecorator.generated.h"

class UWidget;


/**	 Allow create custom widget via blueprint to past it into RichTextBlock
 *	Override CreateWidget for pass widget, use metadata for custom parameters.
 *	Format:
 *		<Widget>TextContent</>
 *		or <Widget Content="TextContent"/>
 */
UCLASS(Abstract)
class TAVAUIUTILS_API UEditableTextWidgetDecorator : public URichEditableTextDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* Owner) override;
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichEditableText* Owner) override;

	FSlateWidgetRun::FWidgetRunInfo CreateDecoratorWidget(const FTextRunInfo& TextRunInfo, const ISlateStyle* SlateStyle);

	UFUNCTION(BlueprintNativeEvent)
	UWidget* CreateWidget(const FTextBlockStyle& TextStyle, const FText& Content, const TMap<FString, FString>& Metadata);
};
