// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "NSHudStructs.generated.h"

UENUM()
enum class EWindowSnap : uint8
{
	None,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};


//window gameplay tags, for save default window positions
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WINDOW_CHAT_SETTINGS);
