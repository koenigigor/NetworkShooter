// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Overlay.h"
#include "QuestFeed.generated.h"



UENUM(BlueprintType)
enum class EFeedChildSnapping : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
	//TopMiddle
	//BottomMiddle...
};


/** Stack child's with 'scrolling' animation on add new widget
 *	intend to use as kill feed or notify feed
 */
UCLASS()
class TAVAUIUTILS_API UQuestFeed : public UOverlay
{
	GENERATED_BODY()
public:
	/** Item snap location */
	UPROPERTY(EditAnywhere)
	EFeedChildSnapping ChildSnapping = EFeedChildSnapping::BottomRight;

	/** Items slide direction */
	UPROPERTY(EditAnywhere)
	bool bHorizontalSlide = true;

	/** Interpolation animation speed */
	UPROPERTY(EditAnywhere)
	float InterpSpeed = 3.f;

	/** Padding between items */
	UPROPERTY(EditAnywhere)
	FVector2D InnerPadding = {100.f, 0.f};

	/** Tick movement animation, must be called outside
	 *	//todo find or implement native tick  */
	UFUNCTION(BlueprintCallable)
	void TickInterpPanels(float DeltaTime);	
	
protected:
	virtual void OnSlotAdded(UPanelSlot* InSlot) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	FVector2D GetSnappingMultiplier();
};
