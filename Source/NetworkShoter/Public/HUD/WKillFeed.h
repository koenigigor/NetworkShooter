// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WKillFeed.generated.h"

class UWKillFeed_Row;

/**
 * Kill Feed widget
 * show pop up when somebody kill somebody
 */
UCLASS()
class NETWORKSHOTER_API UWKillFeed : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
	/** called when some pawn dead */
	UFUNCTION()
	void OnSomebodyKilled(APawn* WhoKilled);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_NewRowCreated(UWKillFeed_Row* NewRow);

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UWKillFeed_Row> RowClass = nullptr;

	bool bConstructed = false;
};
