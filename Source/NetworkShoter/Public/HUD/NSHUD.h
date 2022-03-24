// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "TabMenu.h"
#include "GameFramework/HUD.h"
#include "NSHUD.generated.h"

class UTabMenu;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSHUD : public AHUD
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> PreMatchWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings") //, meta=(BlueprintBaseOnly=true)) not work
	TSubclassOf<UTabMenu> TabMenuClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> PostMatchWidgetClass;
	

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMatchStart();
	
	UFUNCTION()
	void OnMatchEnd();

private:
	UPROPERTY()
	UUserWidget* PreMatchWidget = nullptr;
	
	UPROPERTY()
	UTabMenu* TabMenu = nullptr;
	
	UPROPERTY()
	UUserWidget* PostMatchWidget = nullptr;
};
