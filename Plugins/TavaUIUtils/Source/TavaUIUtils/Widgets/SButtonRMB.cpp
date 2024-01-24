// Fill out your copyright notice in the Description page of Project Settings.


#include "SButtonRMB.h"

void SButtonRMB::Construct(const FArguments& InArgs)
{
	OnClickedRight = InArgs._OnClickedRight;
	
	SButton::Construct
	(
		SButton::FArguments()
		.IsFocusable(InArgs._IsFocusable)
		.ForegroundColor(InArgs._ForegroundColor)
		.OnClicked(InArgs._OnClicked)
		.OnPressed(InArgs._OnPressed)
		.OnReleased(InArgs._OnReleased)
		.OnHovered(InArgs._OnHovered)
		.OnUnhovered(InArgs._OnUnhovered)
		.ClickMethod(InArgs._ClickMethod)
		.TouchMethod(InArgs._TouchMethod)
		.PressMethod(InArgs._PressMethod)
		.HoveredSoundOverride(InArgs._HoveredSoundOverride)
		.PressedSoundOverride(InArgs._PressedSoundOverride)
		.ButtonStyle(InArgs._ButtonStyle)
		.TextStyle(InArgs._TextStyle)
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.ContentPadding(InArgs._ContentPadding)
		.Text(InArgs._Text)
		.DesiredSizeScale(InArgs._DesiredSizeScale)
		.ContentScale(InArgs._ContentScale)
		.ButtonColorAndOpacity(InArgs._ButtonColorAndOpacity)
		.TextShapingMethod(InArgs._TextShapingMethod)
		.TextFlowDirection(InArgs._TextFlowDirection)
		
		//.Content(InArgs._Content)
	);

	
	/*
	// Only do this if we're exactly an SButton
	if (GetType() == SButtonTypeName)
	{
		SetCanTick(false);
	}
	*/

}


void SButtonRMB::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	SButton::OnFocusLost(InFocusEvent);
	ReleaseRight();
}

FReply SButtonRMB::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
	
	if (IsEnabled() && (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton))
	{
		PressRight();
		PressedScreenSpacePositionRight = MouseEvent.GetScreenSpacePosition();

		EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
		
		if(InputClickMethod == EButtonClickMethod::MouseDown)
		{
			//get the reply from the execute function
			Reply = ExecuteOnClickRight();

			//You should ALWAYS handle the OnClicked event.
			ensure(Reply.IsEventHandled() == true);
		}
		else if (InputClickMethod == EButtonClickMethod::PreciseClick)
		{
			// do not capture the pointer for precise taps or clicks
			// 
			Reply = FReply::Handled();
		}
		else
		{
			//we need to capture the mouse for MouseUp events
			Reply = FReply::Handled().CaptureMouse( AsShared() );
		}
	}
	
	return Reply;
}

FReply SButtonRMB::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
	const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;
	const bool bMeetsPressedRequirements = (!bMustBePressed || (bIsPressedRight && bMustBePressed));
	

	if (bMeetsPressedRequirements && (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton))	//todo here problem
	{
		ReleaseRight();

		if ( IsEnabled() )
		{
			if ( InputClickMethod == EButtonClickMethod::MouseDown )
			{
				// NOTE: If we're configured to click on mouse-down/precise-tap, then we never capture the mouse thus
				//       may never receive an OnMouseButtonUp() call.  We make sure that our bIsPressed
				//       state is reset by overriding OnMouseLeave().
			}
			else
			{
				if ( IsHovered() )
				{
					// If we asked for a precise tap, all we need is for the user to have not moved their pointer very far.
					const bool bTriggerForTouchEvent = InputClickMethod == EButtonClickMethod::PreciseClick;

					// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
					// pressed the button down first, then we'll allow the click to proceed without an active capture
					const bool bTriggerForMouseEvent = (InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture() );

					if ( ( bTriggerForTouchEvent || bTriggerForMouseEvent ) )
					{
						Reply = ExecuteOnClickRight();
					}
				}
			}
		}
		
		//If the user of the button didn't handle this click, then the button's
		//default behavior handles it.
		if ( Reply.IsEventHandled() == false )
		{
			Reply = FReply::Handled();
		}

		return Reply;
	}

	return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SButtonRMB::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if ( bIsPressedRight && IsPreciseTapOrClick(MouseEvent) && FSlateApplication::Get().HasTraveledFarEnoughToTriggerDrag(MouseEvent, PressedScreenSpacePositionRight) )
	{
		ReleaseRight();
	}
	
	return SButton::OnMouseMove(MyGeometry, MouseEvent);
}

void SButtonRMB::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	//if ( ClickMethod2 == EButtonClickMethod::MouseDown || IsPreciseTapOrClick(MouseEvent) )
	{
		ReleaseRight();
	}
	
	SButton::OnMouseLeave(MouseEvent);
}

void SButtonRMB::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	//ReleaseRight();
	SButton::OnMouseCaptureLost(CaptureLostEvent);
}

FReply SButtonRMB::ExecuteOnClickRight()
{
	if (OnClickedRight.IsBound())
	{
		FReply Reply = OnClickedRight.Execute();
#if WITH_ACCESSIBILITY
		// @TODOAccessibility: This should pass the Id of the user that clicked the button but we don't want to change the regular Slate API just yet
		FSlateApplicationBase::Get().GetAccessibleMessageHandler()->OnWidgetEventRaised(FSlateAccessibleMessageHandler::FSlateWidgetAccessibleEventArgs(AsShared(), EAccessibleEvent::Activate));
#endif
		return Reply;
	}
	else
	{
		return FReply::Handled();
	}
}

void SButtonRMB::PressRight()
{
	if ( !bIsPressedRight )
	{
		bIsPressedRight = true;
		PlayPressedSound();
		//OnPressedRight.ExecuteIfBound();
		//UpdatePressStateChanged();
	}	
}

void SButtonRMB::ReleaseRight()
{
	if ( bIsPressedRight )
	{
		bIsPressedRight = false;
		//OnReleasedRight.ExecuteIfBound();
		//UpdatePressStateChanged();
	}	
}
