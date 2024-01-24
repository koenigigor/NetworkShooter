// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RichEditableTextDecorator.h"
#include "Framework/Text/SlateWidgetRun.h"
#include "EditableTextRMBDecorator.generated.h"

class UHyperlinkStyleData;


/**	 Create hyperlink buttons with left and right mouse button response.
 *	Format:
 *		<LinkRMB Style="Link.Default" TextColor="(R=0.000000,G=0.000000,B=0.000000,A=0.000000)" UseParentColor="">TextContent</>
 *		or <LinkRMB Style="Link.Default" TextColor="(R=0.000000,G=0.000000,B=0.000000,A=0.000000)" UseParentColor="" Content="TextContent"/>
 *		@Style: Name of style from HyperlinkDecoratorData
 *		@TextColor: Override text color (FLinearColor.ToString())
 *		@UseParentColor: Get text color from owning widget block (URichTextBlock or URichEditableText)
 *		@Content: String who represent hyperlink text
 */
UCLASS(Abstract)
class TAVAUIUTILS_API UEditableTextRMBDecorator : public URichEditableTextDecorator
{
	GENERATED_BODY()
public:
	UEditableTextRMBDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichEditableText* Owner) override;

	FSlateWidgetRun::FWidgetRunInfo CreateDecoratorWidget(const FTextRunInfo& TextRunInfo, const ISlateStyle* SlateStyle);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FButtonStyle DefaultButtonStyle = FButtonStyle::GetDefault();

	UPROPERTY(EditAnywhere, Category="Defaults")
	TObjectPtr<UHyperlinkStyleData> HyperlinkDecoratorData = nullptr;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnClickLeft(const TMap<FString, FString>& MetaData);

	UFUNCTION(BlueprintImplementableEvent)
	void OnClickRight(const TMap<FString, FString>& MetaData);

	//button style setup by ref, so i keep added style
	mutable TArray<FButtonStyle> ButtonStyles;
};

