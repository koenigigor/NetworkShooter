// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatSendMessageWindow.generated.h"

class URichEditableText;

UCLASS(Abstract)
class NETWORKSHOTER_API UChatSendMessageWindow : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FString> ReplaceTagMap;

	
	/** Add text to end of InputTextBlock */
	UFUNCTION(BlueprintCallable)
	void AppendText(const FText& Text);

	/** Set text in InputTextBlock */
	UFUNCTION(BlueprintCallable)
	void SetText(const FText& Text);

	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	URichEditableText* InputText = nullptr;

	
	UFUNCTION()
    void OnTextChanged(const FText& Text);

	//get decorator who must replace tag (like :Smile_1: -> <imd id="Smile_1"/>)
	FString GetReplacerForTag(const FString& Tag);
};
