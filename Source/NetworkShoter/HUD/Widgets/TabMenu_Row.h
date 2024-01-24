// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSStructures.h"
#include "TeamAttitude.h"
#include "Blueprint/UserWidget.h"
#include "TabMenu_Row.generated.h"

class ANSPlayerState;
class UTextBlock;

/**
 * widget for player statistic
 * must be stored in widget "TabMenu"
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UTabMenu_Row : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	void Init(ANSPlayerState* PlayerState = nullptr);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCharacterDead();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnCharacterRespawn();

	//tmp
	UFUNCTION(BlueprintPure)
	EGameTeam GetTeam() const;

protected:
	UFUNCTION()
	void OnCharacterDeath(APawn* DeadPawn);

	UFUNCTION()
	void OnStatisticUpdate();

	TWeakObjectPtr<ANSPlayerState> OwningPlayerState = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerNameText = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* KillCountText = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* DeathCountText = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* AssistCountText = nullptr;
};
