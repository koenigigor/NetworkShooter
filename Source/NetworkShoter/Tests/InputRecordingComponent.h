// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputData.h"
#include "Components/ActorComponent.h"
#include "InputRecordingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UInputRecordingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInputRecordingComponent();

	UPROPERTY(EditAnywhere, Category="Setup")
	FString FileName = "CharacterInputRecord";
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void AppendBindings();

	FString GenerateFilename() const;
	
	FInputData InputData;

	UPROPERTY()
	UInputComponent* InputComponent = nullptr;
	
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent = nullptr;

	UPROPERTY()
	UPlayerInput* PlayerInput = nullptr;

	UPROPERTY()
	UEnhancedPlayerInput* EnhancedPlayerInput = nullptr;
};
