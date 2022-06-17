// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WKillFeed.h"

#include "Game/NSGameState.h"
#include "HUD/WKillFeed_Row.h"

void UWKillFeed::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(NSTag::Chat::Damage(), this, &ThisClass::OnKilled);
}

void UWKillFeed::NativeDestruct()
{
	Super::NativeDestruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);	
}

void UWKillFeed::OnKilled(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{	
	auto Row = CreateWidget<UWKillFeed_Row>(this, RowClass);
	Row->Init(DamageInfo);
	
	//send new row into blueprint
	BP_NewRowCreated(Row);
}
