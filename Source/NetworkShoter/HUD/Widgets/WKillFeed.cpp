// Fill out your copyright notice in the Description page of Project Settings.


#include "WKillFeed.h"
#include "Widgets/QuestFeed.h"

void UWKillFeed::NativeConstruct()
{
	Super::NativeConstruct();

	// Listen kill events for automatically add it
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DEATH, this, &ThisClass::OnKilled);

	//clear preview
	if (Feed) Feed->ClearChildren();
}

void UWKillFeed::NativeDestruct()
{
	Super::NativeDestruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);	
}

void UWKillFeed::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (Feed) Feed->TickInterpPanels(DeltaTime);
}

void UWKillFeed::OnKilled(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{	
	auto Row = CreateWidget<UWRowOnKilled>(this, OnKilledRow);
	Row->Init(DamageInfo);

	check(Feed);
	Feed->AddChildToOverlay(Row);
}
