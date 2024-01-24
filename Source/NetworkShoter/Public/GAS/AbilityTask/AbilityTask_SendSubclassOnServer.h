// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SendSubclassOnServer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSendSubclassDelegate, TSubclassOf<UObject>, ReplicatedSubclass);

/**
 * Replicate TSubclassOf<UObject> on server
 * used if need send send sata from client
 * (like selected special in GA_UseSpecial)
 */
UCLASS()
class NETWORKSHOTER_API UAbilityTask_SendSubclassOnServer : public UAbilityTask
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintAssignable)
	FSendSubclassDelegate Receive;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_SendSubclassOnServer* AbilityTask_SendSubclassOnServer(UGameplayAbility* OwningAbility, TSubclassOf<UObject> InClassToSend);
	
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION()
	void OnClassReplicate(FGameplayAbilitySpecHandle SpecHandle, TSubclassOf<UObject> Class);

	UPROPERTY()
	TSubclassOf<UObject> ClassToSend = nullptr;

	FDelegateHandle DelegateHandle;
};
