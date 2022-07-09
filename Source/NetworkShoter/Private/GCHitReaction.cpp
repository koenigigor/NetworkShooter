// Fill out your copyright notice in the Description page of Project Settings.


#include "GCHitReaction.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCHitReaction, All, All)

bool UGCHitReaction::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	const auto PC = MyTarget->GetInstigatorController<APlayerController>();
	if (PC && PC -> IsLocalController())
	{
		PC->PlayerCameraManager->StartCameraShake(CameraShake);
		PC->PlayerCameraManager->AddGenericCameraLensEffect(LensEffect);
		if (UIEffect)
		{
			CreateWidget(PC, UIEffect)->AddToViewport();
		}
	}
	
	return Super::OnExecute_Implementation(MyTarget, Parameters);
}