// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ButtonRMB.generated.h"

/** Simple button with Right click event */
UCLASS()
class TAVAUIUTILS_API UButtonRMB : public UButton
{
	GENERATED_BODY()
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	FReply SlateHandleClickedRight();

	/** Called when the button is clicked */
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonClickedEvent OnClickedRight;
};
