// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MinimapController.generated.h"

class UMapObjectComponent;

/** Game state component,
 *	notify map widget for add/remove icon/background */
UCLASS( ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UMinimapController : public UActorComponent
{
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintPure, DisplayName = "GetMinimapController", meta = (DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"))
	static UMinimapController* Get(UObject* WorldContextObject);
	
	UMinimapController();

	void ShowIcon(UMapObjectComponent* Icon);
	void RemoveIcon(UMapObjectComponent* Icon);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIconDelegate, UMapObjectComponent*, Icon);
	UPROPERTY(BlueprintAssignable)
	FIconDelegate OnIconAdd;
	UPROPERTY(BlueprintAssignable)
	FIconDelegate OnIconRemove;

	// Active icons
	UPROPERTY(BlueprintReadOnly)
	TArray<UMapObjectComponent*> VisibleMapObjects;

};
