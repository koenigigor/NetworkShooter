// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSStructures.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
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
	virtual void NativeDestruct() override;
	
	/** called when some pawn dead */
	void OnKilled(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_NewRowCreated(UWKillFeed_Row* NewRow);

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UWKillFeed_Row> RowClass = nullptr;

	bool bConstructed = false;

	FGameplayMessageListenerHandle ListenerHandle;
};
