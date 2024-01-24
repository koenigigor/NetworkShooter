// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputData.generated.h"


USTRUCT()
struct FAxisData
{
	GENERATED_BODY()
	FAxisData(){};
	FAxisData(FName InName, float InValue) : Name(InName), Value(InValue){};

	UPROPERTY()
	FName Name;

	UPROPERTY()
	float Value = 0.f;
};

USTRUCT()
struct FActionsData
{
	GENERATED_BODY()
	FActionsData(){};
	FActionsData(FName InName, FKey InKey, bool InState) : Name(InName), Key(InKey), State(InState){};
	
	UPROPERTY()
	FName Name;

	UPROPERTY()
	FKey Key;

	

	UPROPERTY()
	bool State = false;
};

USTRUCT()
struct FEnhancedActionData
{
	GENERATED_BODY()

	FEnhancedActionData(){};
	FEnhancedActionData(FString InName, FVector InValue) : ActionName(InName), Value(InValue){}
	
	UPROPERTY()
	FString ActionName;

	UPROPERTY()
	FVector Value = FVector::ZeroVector;

	bool operator==(const FEnhancedActionData& Other) const
	{
		return Other.ActionName == ActionName;
	}
	bool operator==(const FString& Other) const
	{
		return Other == ActionName;
	}
};

USTRUCT()
struct FBindingsData
{
	GENERATED_BODY()

	UPROPERTY()
	float Time = 0;
	
	UPROPERTY()
	TArray<FAxisData> AxisData;

	UPROPERTY()
	TArray<FActionsData> ActionsData;

	UPROPERTY()
	TArray<FEnhancedActionData> EnhancedActionsData;
};

USTRUCT()
struct FInputData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FTransform InitialTransform;

	UPROPERTY()
	TArray<FBindingsData> BindingsData;
};
