#pragma once

#include "NSStructures.h"
#include "DamageDescriptionInterface.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UDamageDescriptionInterface : public UInterface
{
	GENERATED_BODY()
};

class IDamageDescriptionInterface
{    
	GENERATED_BODY()

public:
	/** Return readable damage message
	 *	like:
	 *	User1 used riffle shot and deal 48 damage on user2
	 *	User1 drink heal potion and receive 17 HP
	 *
	 *	must be implemented in ability, or damage causer (like spawnable turret or mine)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString GetDamageDescription(const FDamageInfo& DamageInfo) const;
};