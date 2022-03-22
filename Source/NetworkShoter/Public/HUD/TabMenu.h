// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TabMenu_Row.h"
#include "Blueprint/UserWidget.h"
#include "TabMenu.generated.h"

class ANSPlayerState;
class ANSGameState;

/**
 * Match Menu with team info
 * pop up on press tab
 */
UCLASS()
class NETWORKSHOTER_API UTabMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTabMenu_Row> TabMenuRowClass;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_RowsUpdated();
	
	virtual void NativeConstruct() override;

	/* Called when controlled pawn Spawned */
	void OnPawnSpawned(ANSPlayerState* PlayerState);
	
	/* Called when controlled pawn Death */
	void OnPawnDeath(ANSPlayerState* PlayerState);

	/** array for store team info, for more team create derived class and add array */
	//TArray<ANSPlayerState> Team0;
	/** array for store player info */
	UPROPERTY(BlueprintReadOnly)
	TArray<UTabMenu_Row*> Rows;

	
	/** return time in seconds from start match */
	UFUNCTION(BlueprintPure)
	float GetMatchTime();

	/** return string format "Waiting match 0:30" or "match in progress 1:27" */
	UFUNCTION(BlueprintPure)
	FString GetMatchStatusAndTime();
	
	/** return percent when match time end
	 *	@0 Start
	 *	@1 End
	 */
	UFUNCTION(BlueprintPure)
	float GetMatchTimeProgress();

	ANSGameState* GameState = nullptr;
};
