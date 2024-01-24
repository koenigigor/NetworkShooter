// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat/ChatController.h"
#include "ChatScrollBox.generated.h"

class UChatController;
class ANSPlayerState;

/** ScrollBox for aggregate chat messages */
UCLASS(Abstract, meta = (DisableNativeTick))
class NETWORKSHOTER_API UChatScrollBox : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	int32 TabID;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnReceiveMessage(const FChatMessage& Message);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnReceiveMessage(const FChatMessage& Message);
};
