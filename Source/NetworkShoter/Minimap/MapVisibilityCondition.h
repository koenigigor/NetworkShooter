

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapVisibilityCondition.generated.h"

/** Visibility condition for map elements (Location, layer, icon) */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, Blueprintable)
class NETWORKSHOTER_API UMapVisibilityCondition : public UObject
{
	GENERATED_BODY()
public:
	virtual UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BeginPlay();
	
	UFUNCTION(BlueprintNativeEvent)
	bool IsVisible() const;
	
	/** Manually notify when update visibility */
	UFUNCTION(BlueprintCallable)
	void Update();

	FSimpleDelegate OnVisibilityUpdate;
};
