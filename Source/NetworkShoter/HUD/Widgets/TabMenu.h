// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabMenu.generated.h"

class UTabMenu_Row;

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

	void UpdateRows();

	/** array for store team info, for more team create derived class and add array */
	//TArray<ANSPlayerState> Team0;
	/** array for store player info */
	UPROPERTY(BlueprintReadOnly)
	TArray<UTabMenu_Row*> Rows;
};
