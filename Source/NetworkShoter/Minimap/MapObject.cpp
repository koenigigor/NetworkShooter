// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObject.h"

#include "Components/Image.h"


FVector UMapObject::GetWorldLocation() const
{
	//todo, cache once?
	if (const auto MapObjectComponent = Cast<UActorComponent>(GetOuter()))
	{
		return MapObjectComponent->GetOwner()->GetActorLocation();
	}
	
	return InitialLocation;
}

UWidget* UMapObject::CreateWidget_Implementation()
{
	return nullptr;
}

const FString& UMapObject::GetUniqueName() const
{
	if (UniqueName.IsEmpty())
	{
		//get unique name in format ActorName.ComponentName (it unique)
		GetOuter()->GetPathName(GetOuter()->GetOuter()->GetOuter(), UniqueName);
	}
	
	return UniqueName;
}

///

UWidget* UMapObjectSimpleImage::CreateWidget_Implementation()
{
	//create simple Image
	const auto ImageWidget = NewObject<UImage>(this);
	ImageWidget->SetBrushResourceObject(Image);
	ImageWidget->SetDesiredSizeOverride(FVector2D(Size));
	ImageWidget->Brush.SetImageSize(FVector2D(Size));
	
	return ImageWidget;
}

///

UMapObjectWrapper* UMapObjectWrapper::AddBaked(UMapObject* MapObject)
{
	BakedObject = MapObject;
	
	if (Name.IsEmpty())
	{
		Name = MapObject->GetUniqueName();
	}

	MapObject->OnLayerChange.AddLambda([&](UMapObject* MapObject_)
    {
    	OnLayerChange.Broadcast(this);
    });
	
	return this;
}

UMapObjectWrapper* UMapObjectWrapper::AddExternal(UMapObject* MapObject)
{
	ExternalObject = MapObject;
	
	if (Name.IsEmpty())
	{
		Name = MapObject->GetUniqueName();
	}

	MapObject->OnLayerChange.AddLambda([&](UMapObject* MapObject_)
	{
		OnLayerChange.Broadcast(this);
	});
	
	return this;
}

UMapObjectWrapper* UMapObjectWrapper::AddRuntime(UMapObject* MapObject)
{
	RuntimeObject = MapObject;
	
	if (Name.IsEmpty())
	{
		Name = MapObject->GetUniqueName();
	}

	MapObject->OnLayerChange.AddLambda([&](UMapObject* MapObject_)
	{
		OnLayerChange.Broadcast(this);
	});
	
	return this;
}

void UMapObjectWrapper::RemoveBaked()
{
	if (BakedObject)
	{
		BakedObject = nullptr;
	}
}

void UMapObjectWrapper::RemoveExternal()
{
	if (ExternalObject)
	{
		ExternalObject = nullptr;
	}
}

void UMapObjectWrapper::RemoveRuntime()
{
	if (RuntimeObject)
	{
		RuntimeObject = nullptr;
	}
}
