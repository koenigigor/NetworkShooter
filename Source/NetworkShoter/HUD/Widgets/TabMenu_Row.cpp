// Fill out your copyright notice in the Description page of Project Settings.


#include "TabMenu_Row.h"

#include "Components/TextBlock.h"
#include "Game/NSPlayerState.h"

void UTabMenu_Row::NativeConstruct()
{
	Super::NativeConstruct();

	//check player if dead
	if (OwningPlayerState.IsValid())
	{
		if (OwningPlayerState->bDeath)
		{
			BP_OnCharacterDead();
		}
		else
		{
			BP_OnCharacterRespawn();
		}
	} 
}

void UTabMenu_Row::Init(ANSPlayerState* PlayerState)
{
	if (!PlayerState || OwningPlayerState == PlayerState) return;

	check(PlayerNameText);
    PlayerNameText->SetText(FText::FromString(PlayerState->GetPlayerName().Mid(0, 10)));	//todo tmp clamp 10 symbols
	
	PlayerState->PlayerStatisticUpdateDelegate.AddDynamic(this, &UTabMenu_Row::OnStatisticUpdate);
	PlayerState->CharacterDeadDelegate.AddDynamic(this, &UTabMenu_Row::OnCharacterDeath);
	PlayerState->CharacterRespawnDelegate.AddDynamic(this, &UTabMenu_Row::BP_OnCharacterRespawn);
	OwningPlayerState = MakeWeakObjectPtr(PlayerState);

	OnStatisticUpdate();
}

EGameTeam UTabMenu_Row::GetTeam() const
{
	return OwningPlayerState.IsValid() ? OwningPlayerState->GetTeamID() : EGameTeam::Neutral;
}

void UTabMenu_Row::OnCharacterDeath(APawn* DeadPawn)
{
	BP_OnCharacterDead();
}

void UTabMenu_Row::OnStatisticUpdate()
{
	check(KillCountText);
	check(DeathCountText);
	check(AssistCountText);

	if (OwningPlayerState.IsValid())
	{
		const auto Statistic = OwningPlayerState->GetPlayerStatistic();
		FNumberFormattingOptions NumberFormatOptions;
		NumberFormatOptions.AlwaysSign = false;
		NumberFormatOptions.UseGrouping = false;
		NumberFormatOptions.MinimumIntegralDigits = 2;
		NumberFormatOptions.MaximumIntegralDigits = 2;

		KillCountText->SetText(FText::AsNumber(Statistic.KillCount, &NumberFormatOptions));
		DeathCountText->SetText(FText::AsNumber(Statistic.DeathCount, &NumberFormatOptions));
		AssistCountText->SetText(FText::AsNumber(Statistic.AssistCount, &NumberFormatOptions));
	}
}
