#include "MapVisibilityCondition.h"

UWorld* UMapVisibilityCondition::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld) return nullptr;
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

void UMapVisibilityCondition::PostInitProperties()
{
	UObject::PostInitProperties();

	if (GetOuter() && GetOuter()->GetWorld())
	{
		//@future: maybe, if many conditions, have reason call begin play on first visibility request?
		BeginPlay();
	}
}

bool UMapVisibilityCondition::IsVisible_Implementation() const
{
	return true;
}

void UMapVisibilityCondition::Update()
{
	OnVisibilityUpdate.Execute();
}

