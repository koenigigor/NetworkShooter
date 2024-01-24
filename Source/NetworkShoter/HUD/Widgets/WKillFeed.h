// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSStructures.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "WKillFeed.generated.h"

class UQuestFeed;


/** Row for WKillFeed, has info about kill */
UCLASS(Abstract)
class NETWORKSHOTER_API UWRowOnKilled : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Init(FDamageInfo KillInfo);
};


/**
 * Kill Feed widget
 * show pop up when somebody kill somebody
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UWKillFeed : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/** Bind to any Player death, create UWKillFeed_Row notification widget */
	void OnKilled(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<UWRowOnKilled> OnKilledRow = nullptr;

	bool bConstructed = false;

	FGameplayMessageListenerHandle ListenerHandle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UQuestFeed* Feed = nullptr;
};
