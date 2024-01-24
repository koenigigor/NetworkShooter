// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HyperlinkStyleData.generated.h"



/** Preset of hyperlink styles, used by RichEditableText hyperlink decorator */
UCLASS()
class TAVAUIUTILS_API UHyperlinkStyleData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FHyperlinkStyle> Styles;
};
