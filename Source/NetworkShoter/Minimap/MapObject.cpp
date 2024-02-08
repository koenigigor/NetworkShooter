// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObject.h"

#include "Components/Image.h"


FVector UMapObject::GetWorldLocation() const
{
	//todo, virtual, movable flag, cache once?
	if (const auto MapObjectComponent = Cast<UActorComponent>(GetOuter()))
	{
		return MapObjectComponent->GetOwner()->GetActorLocation();
	}

	return InitialLocation;
}

UWidget* UMapObject::CreateWidget_Implementation(APlayerController* PC)
{
	return nullptr;
}

UWorld* UMapObject::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld) return nullptr;
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
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

UWidget* UMapObjectSimpleImage::CreateWidget_Implementation(APlayerController* PC)
{
	//create simple Image
	const auto ImageWidget = NewObject<UImage>(this);
	ImageWidget->SetBrushResourceObject(Image);
	ImageWidget->SetDesiredSizeOverride(FVector2D(Size));
	ImageWidget->Brush.SetImageSize(FVector2D(Size));

	return ImageWidget;
}

///

UMapObjectContainer* UMapObjectContainer::Add(UMapObject* MapObject, EMapObjectType Type)
{
	if (!MapObject) return this;
	
	switch (Type)
	{
		case Baked:
			BakedObject = MapObject;
			break;
		case External:
			ExternalObject = MapObject;
			break;
		case Runtime:
			RuntimeObject = MapObject;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("UMapObjectContainer::AddMapObject unknown type"))
	}

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

void UMapObjectContainer::Remove(EMapObjectType Type)
{
	switch (Type)
	{
		case Baked:
			BakedObject = nullptr;
		break;
		case External:
			ExternalObject = nullptr;
		break;
		case Runtime:
			RuntimeObject = nullptr;
		break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("UMapObjectContainer::RemoveMapObject unknown type"))
	}	
}

UMapObjectContainer* UMapObjectContainer::AddBaked(UMapObject* MapObject)
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

UMapObjectContainer* UMapObjectContainer::AddExternal(UMapObject* MapObject)
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

UMapObjectContainer* UMapObjectContainer::AddRuntime(UMapObject* MapObject)
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

void UMapObjectContainer::RemoveBaked()
{
	if (BakedObject)
	{
		BakedObject = nullptr;
	}
}

void UMapObjectContainer::RemoveExternal()
{
	if (ExternalObject)
	{
		ExternalObject = nullptr;
	}
}

void UMapObjectContainer::RemoveRuntime()
{
	if (RuntimeObject)
	{
		RuntimeObject = nullptr;
	}
}

UMapObject* UMapObjectContainer::Get() const
{
	if (RuntimeObject) return RuntimeObject;
	if (ExternalObject) return ExternalObject;
	if (BakedObject) return BakedObject;
	return nullptr;
}
