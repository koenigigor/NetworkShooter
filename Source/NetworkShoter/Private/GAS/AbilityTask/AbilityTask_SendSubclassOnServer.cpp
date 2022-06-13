// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/AbilityTask_SendSubclassOnServer.h"

#include "GAS/NSAbilitySystemComponent.h"

UAbilityTask_SendSubclassOnServer* UAbilityTask_SendSubclassOnServer::AbilityTask_SendSubclassOnServer(
	UGameplayAbility* OwningAbility, TSubclassOf<UObject> InClassToSend)
{
	auto Task = NewAbilityTask<UAbilityTask_SendSubclassOnServer>(OwningAbility);
	Task->ClassToSend = InClassToSend;
	return Task;
}

void UAbilityTask_SendSubclassOnServer::Activate()
{
	Super::Activate();

	UE_LOG(LogTemp, Display, TEXT("UAbilityTask_SendSubclassOnServer::Activate"))

	if (const auto ASC = Cast<UNSAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (GetOwnerActor()->HasAuthority())
		{
			UE_LOG(LogTemp, Display, TEXT("ASC->OnClassReplicate.AddUObject"))
			DelegateHandle = ASC->OnClassReplicate.AddUObject(this, &ThisClass::OnClassReplicate);
		}
		else
		{
			if (ClassToSend)
			{
				UE_LOG(LogTemp, Display, TEXT("ASC->ServerReplicateSubclass(GetAbilitySpecHandle(), ClassToSend)"))
				ASC->ServerReplicateSubclass(GetAbilitySpecHandle(), ClassToSend);
			}
		}
	}
}

void UAbilityTask_SendSubclassOnServer::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	
	if (const auto ASC = Cast<UNSAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->OnClassReplicate.Remove(DelegateHandle);
	}
}

void UAbilityTask_SendSubclassOnServer::OnClassReplicate(FGameplayAbilitySpecHandle SpecHandle,
                                                         TSubclassOf<UObject> Class)
{
	if (SpecHandle == GetAbilitySpecHandle())
	{
		Receive.Broadcast(Class);
		EndTask();
	}
}
