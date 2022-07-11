// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "TabMenu.h"
#include "GameFramework/HUD.h"
#include "NSHUD.generated.h"

class UTabMenu;
class UHUDWidget;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSHUD : public AHUD
{
	GENERATED_BODY()

	/** Hud widget who shows always */
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> SharedHUDClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> PreMatchWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UHUDWidget> CharacterHUDClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UHUDWidget> SpectatorHUDClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Settings") //, meta=(BlueprintBaseOnly=true)) not work
	TSubclassOf<UTabMenu> TabMenuClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> PostMatchWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UUserWidget> PauseMenuClass;
	

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMatchStart();
	
	UFUNCTION()
	void OnMatchEnd();

public:
	/** Triggered when owner possess, in character/in spectator */
	void OnPossess(APawn* InPawn);
	
public:
	UFUNCTION(BlueprintCallable)
	void ShowTabMenu(const bool bShow = true);
	
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu(bool OnPause);

	
private:
	UPROPERTY()
	UUserWidget* CurrentActiveHUD = nullptr;

	UPROPERTY()
	UUserWidget* SharedHUD = nullptr;
	
	UPROPERTY()
	UUserWidget* PreMatchWidget = nullptr;

	UPROPERTY()
	UHUDWidget* CharacterHUD = nullptr;

	UPROPERTY()
	UHUDWidget* SpectatorHUD = nullptr;
	
	UPROPERTY()
	UTabMenu* TabMenu = nullptr;
	
	UPROPERTY()
	UUserWidget* PostMatchWidget = nullptr;

	UPROPERTY()
	UUserWidget* PauseMenu = nullptr;
};
