#include "WrapWidgetDecorator.h"


TSharedRef< FWrapWidgetDecorator > FWrapWidgetDecorator::Create( FString InRunName, const FCreateWidget& InCreateWidgetDelegate )
{
	return MakeShareable( new FWrapWidgetDecorator( InRunName, InCreateWidgetDelegate ) );
}

FWrapWidgetDecorator::FWrapWidgetDecorator( FString InRunName, const FCreateWidget& InCreateWidgetDelegate )
	: RunName( InRunName )
	, CreateWidgetDelegate( InCreateWidgetDelegate )
{

}

bool FWrapWidgetDecorator::Supports( const FTextRunParseResults& RunParseResult, const FString& Text ) const
{
	return ( RunParseResult.Name == RunName );
}

TSharedRef< ISlateRun > FWrapWidgetDecorator::Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style)
{
	FTextRange ModelRange;
	ModelRange.BeginIndex = InOutModelText->Len();
	//*InOutModelText += TEXT('\u00A0'); // Zero-Width Breaking Space // not work with wrap text
	*InOutModelText += TEXT('\u0085');	// this symbol work well with wrap text (find after manually check almost all symbols)
	ModelRange.EndIndex = InOutModelText->Len();

	FTextRunInfo RunInfo( RunParseResult.Name, FText::FromString( OriginalText.Mid( RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex ) ) );
	for(const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
	{
		RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid( Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
	}

	return FSlateWidgetRun::Create(TextLayout, RunInfo, InOutModelText, CreateWidgetDelegate.Execute(RunInfo, Style), ModelRange);
}
