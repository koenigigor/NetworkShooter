// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonRMB.h"

#include "SButtonRMB.h"
#include "Components/ButtonSlot.h"

TSharedRef<SWidget> UButtonRMB::RebuildWidget()
{
	MyButton = SNew(SButtonRMB)
	.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
	.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
	.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
	.OnHovered_UObject( this, &ThisClass::SlateHandleHovered )
	.OnUnhovered_UObject( this, &ThisClass::SlateHandleUnhovered )
	.ButtonStyle(&WidgetStyle)
	.ClickMethod(ClickMethod)
	.TouchMethod(TouchMethod)
	.PressMethod(PressMethod)
	.IsFocusable(IsFocusable)
	.OnClickedRight(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClickedRight))
	;

	if ( GetChildrenCount() > 0 )
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}
	
	return MyButton.ToSharedRef();
}

FReply UButtonRMB::SlateHandleClickedRight()
{
	OnClickedRight.Broadcast();

	return FReply::Handled();
}
