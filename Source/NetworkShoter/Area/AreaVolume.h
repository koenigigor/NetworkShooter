// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AreaVolume.generated.h"

class UAreaListenerComponent;

/** Intend this component attached to trigger volume */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class NETWORKSHOTER_API UAreaVolume : public UActorComponent
{
	GENERATED_BODY()
public:
	UAreaVolume();
	virtual void BeginPlay() override;

	UFUNCTION() void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION() void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	const TArray<TWeakObjectPtr<UAreaListenerComponent>>& GetOverlappedListeners(){ return OverlappedListeners; };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag LocationTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

private:
	//cache overlapped listeners for easy access 
	TArray<TWeakObjectPtr<UAreaListenerComponent>> OverlappedListeners;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UAreaListenerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UAreaListenerComponent();
	UAreaVolume* GetCurrentVolume() const;

	/** @param OutActors actors who contain in same area
	 *	@param bOnlyHighPriority this area must be top priority */
	void GetActorsInCurrentArea(TArray<AActor*>& OutActors, bool bOnlyHighPriority = true) const;

	template <class T>
	void GetActorsInCurrentArea(TArray<T*>& OutActors, bool bOnlyHighPriority = true) const
	{
		TArray<AActor*> Actors;
		GetActorsInCurrentArea(Actors, bOnlyHighPriority);

		for (const auto& Actor : Actors)
		{
			if (const auto CastedActor = Cast<T>(Actor))
			{
				OutActors.Add(CastedActor);
			}
		}		
	}
	
	void AddOverlappedArea(UAreaVolume* Area);
	void RemoveOverlappedArea(UAreaVolume* Area);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAreaChangeDelegate);
	UPROPERTY(BlueprintAssignable)
	FOnAreaChangeDelegate OnAreaChange;
	
protected:
	//Called after change area and delay
	void AreaChanged();
	
	//notify enter volume after exit from high priority volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNotifyEnterOnExit = false;
	
	TArray<TWeakObjectPtr<UAreaVolume>> OverlappedAreaVolumes;

	//notify change area After delay
	FTimerHandle NotifyAreaChangeTimer;	
};
