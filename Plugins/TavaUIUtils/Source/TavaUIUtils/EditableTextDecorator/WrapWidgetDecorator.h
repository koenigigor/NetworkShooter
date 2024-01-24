#pragma once
#include "CoreMinimal.h"
#include "Framework/Text/ITextDecorator.h"
#include "Framework/Text/SlateWidgetRun.h"


/** copy of SRichTextBlock::WidgetDecorator
 *	but change: InOutModelText from '\u00A0' to '\u0085'
 *	who well works with wrap in not editable text, if no spaces between decorators
 *	* Can't override because constructor is private
 */
class TAVAUIUTILS_API FWrapWidgetDecorator : public ITextDecorator
{
public:
	DECLARE_DELEGATE_RetVal_TwoParams( FSlateWidgetRun::FWidgetRunInfo, FCreateWidget, const FTextRunInfo& /*RunInfo*/, const ISlateStyle* /*Style*/ )

	static TSharedRef< FWrapWidgetDecorator > Create( FString InRunName, const FCreateWidget& InCreateWidgetDelegate );

	virtual bool Supports( const FTextRunParseResults& RunParseResult, const FString& Text ) const override;
	virtual TSharedRef< ISlateRun > Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style) override;

protected:
	FWrapWidgetDecorator( FString InRunName, const FCreateWidget& InCreateWidgetDelegate );

	FString RunName;
	FCreateWidget CreateWidgetDelegate;
};