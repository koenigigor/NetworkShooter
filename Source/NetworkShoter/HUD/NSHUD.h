// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NSHudStructs.h"
#include "GameFramework/HUD.h"
#include "NSHUD.generated.h"

class UTabMenu;
class UHUDWidget;
class UWindowLayerWidget;
class UWidget;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HUD", meta=(BlueprintInternalUseOnly))
	static ANSHUD* GetNSHUD(APlayerController* PC);
	
	bool HasWindowInFocus() const;
	bool HasChatFocus() const;
	
protected:
	/** Hud widget who shows always */
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UHUDWidget> SharedHUDClass;
	
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

	void OnPreMatch();
	
	UFUNCTION()
	void OnMatchStart();
	
	UFUNCTION()
	void OnMatchEnd();

public:
	UFUNCTION(BlueprintCallable)
	void ShowTabMenu(const bool bShow = true);
	
protected:
	UFUNCTION()
	void OnPossess(APawn* OldPawn, APawn* NewPawn);
	UFUNCTION()
	void TogglePauseMenu(bool OnPause);
	void OnChangeState(FName NewState);

	// notify player controller for update input mode
	UFUNCTION(BlueprintCallable) //tmp
	void WantUpdateMouse();
	
private:
	UPROPERTY()
	UUserWidget* CurrentActiveHUD = nullptr;

	UPROPERTY()
	UHUDWidget* SharedHUD = nullptr;
	
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

public:
	UFUNCTION(BlueprintCallable)
	void PushWindow(FGameplayTag WindowTag, UUserWidget* Window);
	UFUNCTION(BlueprintCallable)
	void PushChildWindow(UWidget* Parent, UUserWidget* Window, EWindowSnap ParentSnap, bool bHorizontal = true);
	UFUNCTION(BlueprintCallable)
	void RemoveTopWindow();
	UFUNCTION(BlueprintCallable)
	void RemoveAllWindows();

#pragma region ContextMenu
	/** @brief not use FSlateApplication::Get().PushMenu because it focus widget, who broke hybrid input mode */
	/** Context menus should return OnMouseButtonDown Handled, else it will be closed on any unhandled click */
public:
	/** Empty widget with canvas */
	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UWindowLayerWidget> ContextMenuLayerClass;

	/** Push context menu at mouse location */
	UFUNCTION(BlueprintCallable)
	void PushContextMenu(UWidget* Menu);
	UFUNCTION(BlueprintCallable)
	void PushContextMenuAroundWidget(UWidget* Parent, UWidget* Menu, EWindowSnap ParentSnap, bool bHorizontal = true);

	UFUNCTION()
	void OnVisibleMouseClick();
	UFUNCTION()
	void OnSetInputModeGameOnly();
	
private:
	UPROPERTY()
	UWindowLayerWidget* ContextMenuLayer = nullptr;
#pragma endregion ContextMenu

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<UWindowLayerWidget> WindowLayerWidgetClass = nullptr;

	UPROPERTY()
	UWindowLayerWidget* WindowLayerWidget = nullptr;
};
