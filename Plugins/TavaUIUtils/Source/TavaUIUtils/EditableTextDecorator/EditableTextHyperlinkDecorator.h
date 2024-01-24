// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RichEditableTextDecorator.h"
#include "Framework/Text/SlateWidgetRun.h"
#include "EditableTextHyperlinkDecorator.generated.h"

class UHyperlinkStyleData;


/**	 Simple not editable hyperlink decorator
 *	Format:
 *		<a Style="Link.Rare" TextColor="(R=0.000000,G=0.000000,B=0.000000,A=0.000000)">TextContent</>
 *		<a Style="Link.Rare" TextColor="(R=0.000000,G=0.000000,B=0.000000,A=0.000000)" Content="TextContent"/>
 *		@Style Hyperlink style from HyperlinkDecoratorData
 *		@TextColor: Override text color (FLinearColor.ToString())
 *		@Content: String who represent hyperlink text
 */
UCLASS(Abstract)
class TAVAUIUTILS_API UEditableTextHyperlinkDecorator : public URichEditableTextDecorator
{
	GENERATED_BODY()
public:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichEditableText* Owner) override;
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	FSlateWidgetRun::FWidgetRunInfo CreateDecoratorWidget(const FTextRunInfo& RunInfo, const ISlateStyle* Style);

	void HandleClick(const TMap<FString, FString>& Metadata) const;
	
	UPROPERTY(EditAnywhere, Category="Decorator")
	TObjectPtr<UHyperlinkStyleData> HyperlinkDecoratorData = nullptr;
};
