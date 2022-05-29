#if WITH_AUTOMATION_TESTS

#include "NSTestUtils.h"

UWorld* NSTestUtils::GetTestWorld()
{
	{
		const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
		for ( const FWorldContext& Context : WorldContexts )
		{
			if ( (  Context.WorldType == EWorldType::PIE  || Context.WorldType == EWorldType::Game ) && Context.World() )
			{
				return Context.World();
			}
		}
	
		return nullptr;
	}	
}

#endif
