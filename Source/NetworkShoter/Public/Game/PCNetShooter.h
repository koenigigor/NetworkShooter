// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCNetShooter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLocalPauseDeledate, bool, bOnPause);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeStateDelegate, FName NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNSSimpleDelegate);

class ANSHUD;
class UChatProxy;
class UInputAction;


UCLASS()
class NETWORKSHOTER_API APCNetShooter : public APlayerController
{
	GENERATED_BODY()
public:
	APCNetShooter();
		
	virtual void SetupInputComponent() override;
public:
	/** For Process hold abilities in AbilitySystem */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ClientOnPossess(APawn* InPawn);

	virtual void AcknowledgePossession(APawn* P) override;

	/** Called from hud to update input mode (GameAndUI or GameOnly) */
	UFUNCTION(BlueprintCallable)
	void UpdateMouseState();

	/** Special version of Show mouse, for assign to button, set additional flag. for block hide mouse outside if button still pressed */
	UFUNCTION(BlueprintCallable)
	void ShowMouseByButton(bool bShow);
	bool bShowMousePressed = false;

	virtual void SetInputMode(const FInputModeDataBase& InData) override;

	
	UPROPERTY(EditDefaultsOnly, Category="Settings - Input")
	UInputAction* VisibleMouseClickAction = nullptr;

	/** used for notify hud (for hide context menus), triggered by input  */
	void OnVisibleMouseClick();

	/** Input mode with ui (Game and UI or UI only) */
	UPROPERTY(BlueprintAssignable)
	FNSSimpleDelegate OnSetInputModeUI;
	UPROPERTY(BlueprintAssignable)
	FNSSimpleDelegate OnSetInputModeGameOnly;
	UPROPERTY(BlueprintAssignable)
	FNSSimpleDelegate OnVisibleMouseClickNotify;
	
	/**	client version of pause, disable input and show main menu */
	UFUNCTION(BlueprintCallable)
	void ToggleLocalPause();
	
	UPROPERTY(BlueprintAssignable)
	FLocalPauseDeledate OnLocalPause;

	FOnChangeStateDelegate OnChangeState;
	
	//~==============================================================================================
	// Proxy functions
	
	/** Proxy component for send chat message from client to server */
	UPROPERTY()
	UChatProxy* ChatProxy = nullptr;

	/** Proxy function for vote for next map (From vote next map widget) */
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void VoteForMap(FName MapRow, bool Up = true);

	virtual void ChangeState(FName NewState) override;
protected:
	virtual void BeginPlayingState() override;
	virtual void EndPlayingState() override;
	TWeakObjectPtr<APawn> PreviousPawn = nullptr;
	
private:	
	bool bOnPause = false;
	FIntPoint LastMouseLocation = FIntPoint::ZeroValue;
};
