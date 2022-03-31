// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameSession.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void ANSGameSession::RegisterServer()
{
	Super::RegisterServer();
	
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (!OnlineSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSubsystem not found"))
		return;
	}

	IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();  

	FOnlineSessionSettings Settings;

	Settings.bIsDedicated = true;
	Settings.bAllowJoinInProgress = true;
	Settings.bShouldAdvertise=true;
	Settings.bIsLANMatch = true;
	
	Session->CreateSession(0, GameSessionName, Settings);

	UE_LOG(LogTemp, Log, TEXT("-=- Session create -=-"))
}
