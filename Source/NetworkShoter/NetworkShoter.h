// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define LOG_NET_ROLE_STR *(GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer) ? FString("Server") : GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName())
