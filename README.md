# NetworkShooter

My Unreal Engine learning project, with Gameplay Ability System and Networking

<a name="table-of-contents"></a>
## Table of Contents

> 1. [TavaUIUtils plugin](#UIUtils_Plugin)  
>    1.1. [ButtonRMB](#UIUtils_Plugin_ButtonRMB)  
>    1.2. [QuestFeed](#UIUtils_Plugin_QuestFeed)  
>    1.3. [RichEditableText](#UIUtils_Plugin_RichEditableText)  
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.3.1. [Decorators](#UIUtils_Plugin_Decorators)   
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.3.2. [Clipboard copy](#UIUtils_Plugin_CopyDecoratorText)  
>    1.4. [Operations](#UIUtils_Plugin_Operations)  
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.4.1. [Resize Window](#UIUtils_Plugin_Operations_Resize)   
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.4.2. [Move Window](#UIUtils_Plugin_Operations_Move)   
> 2. [NetworkShooter project](#NS_Main)  
>    2.1. [Chat](#NS_Chat)  
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2.1.1. [Chat Controller](#NS_Chat_Controller)   
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2.1.2. [Chat Style](#NS_Chat_Style)   
>    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2.1.3. [Chat Widget](#NS_Chat_Widgets)  
>    2.2. [Minimap](#NS_Minimap)  
> 3. [NetworkShooter project (old)](#NS_Main_Old)



<a name="UIUtils_Plugin"></a>
## 1. TavaUIUtils plugin
My plugin with extended functionality of slate and umg widgets.

<a name="UIUtils_Plugin_ButtonRMB"></a>
#### 1.1. ButtonRMB
A simple UButton and SButton derived widgets with Right mouse button click event.

<a name="UIUtils_Plugin_QuestFeed"></a>
#### 1.2. QuestFeed
A panel widget with nice smoothly child's interpolation, useful for kill feed, or some type of notification feed.
>Preview updates on pose edit property, and resets on save blueprint. If you want how mark children's dirty for save please tell me.

![settings](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/QuestFeedSettings.png)
![demo.gif](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/QuestFeed.gif)

<a name="UIUtils_Plugin_RichEditableText"></a>
#### 1.3. RichEditableText
Editable text with support decorators, in plugin presented several decorators, such as image and button decorator.

![demo.gif](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/RichEditText.gif)

<a name="UIUtils_Plugin_Decorators"></a>
#### 1.3.1. Decorators
Rich editable text decorators compatible with rich text blocks.\
Widget decorators who use with RichTextBlock, use custom FWrapWidgetDecorator because default FWidgetDecorator use "non-breaking space" and it cannot wrap text if use multiple decorators without spaces.

Presented decorators:
* UEditableTextHyperlinkDecorator - In text button with click event.
* UEditableTextRMBDecorator - In text button with left and right click event.
* UEditableTextImageDecorator - In text image.
* UEditableTextWidgetDecorator - Decorator with ability pass your own widgets via blueprint.

For info about metadata used with decorator see .h file of decorator.

<a name="UIUtils_Plugin_CopyDecoratorText"></a>
#### 1.3.2. Clipboard copy decorator text
Ih you want get decorator text (like for copy string into clipboard)
I recommend use metadata tag "ClipboardOverride" for string who be added to clipboard instead default decorator "non-breaking space".
Then build engine from source and override function GetRangeAsTextFromLine in TextLayout.cpp for put string from this metadata tag.

```c++
void GetRangeAsTextFromLine(FString& DisplayText, const FTextLayout::FLineModel& LineModel, const FTextRange& Range)
{
	for (int32 RunIndex = 0; RunIndex < LineModel.Runs.Num(); RunIndex++)
	{
		const FTextLayout::FRunModel& Run = LineModel.Runs[RunIndex];
		const FTextRange& RunRange = Run.GetTextRange();

		const FTextRange IntersectRange = RunRange.Intersect(Range);

		if (!IntersectRange.IsEmpty())
		{
			if (Run.GetRun()->GetRunInfo().MetaData.Contains("ClipboardOverride"))
			{
				DisplayText.Append(Run.GetRun()->GetRunInfo().MetaData["ClipboardOverride"]);
			}
			else
			{
				Run.AppendTextTo(DisplayText, IntersectRange);
			}
		}
		else if (RunRange.BeginIndex > Range.EndIndex)
		{
			//We're past the selection range so we can stop
			break;
		}
	}
}
```

Use custom TextLayout and override it in self created text block I see ineffective, because then will be need override almost all base text widgets (RichText, MultiLineRichText, etc...)

<a name="UIUtils_Plugin_Operations"></a>
#### 1.4. Operations

<a name="UIUtils_Plugin_Operations_Resize"></a>
#### 1.4.1. Resize window
Generic operation for resize window, has 2 variants, simple and with callback on drop. Expected but not required that window widget place on canvas panel.

Use as ordinary drag drop operation. Just create collision in your widget, and start operation when it necessary.

![ResizeWindowOperation](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/ResizeWindow.png)

<a name="UIUtils_Plugin_Operations_Move"></a>
#### 1.4.2. Move window
Operation for move widget on canvas, also has simple and with callback operation.

Option update anchors snap widget anchor to closest canvas edge.

![MoveWindowOperation](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/MoveWindowOperation.png)


**[⬆ Back to Top](#table-of-contents)**

<a name="NS_Main"></a>
## 2. Network Shooter project
Work on this learning project was started a long ago based on well known Network Shooter streams. 
And very soon it was go away from it, to my independent learning project for fun and in-depth study of different mechanics.
Such as GAS, Chat with rich text and link items, and other...

<a name="NS_Chat"></a>
### 2.1. Chat
NS Chat is my interpretation of chat for mmo game. 
In base I get idea of chat from Blade and Soul (like different font settings for each channel per tab). 
And Base things for every MMO-game like Link everything (items, skills, players, bosses, etc...)

<a name="NS_Chat_Controller"></a>
#### 2.1.1. Chat Controller
Chat controller is component responsible for receiving/sending/routing messages.
Component must be plug into GameState class.

Message send by function `SendMessage`. If called from client it redirect message to server via ChatProxy component attached to player controller.
Then server looks recipients for this message in `GetRecipientsForMessage` then message will be send directly to specific players, or broadcast all on nothing found.
Then on target clients will be triggered `SendMessageLocal` for receive message and notify assigned systems (like UI chat window).

<a name="NS_Chat_Style"></a>
#### 2.1.2. Chat Style
Chat style is save game object who response for ui style (Windows, Tabs, channel colors...).
Save initialized from ChatController.

`DefaultChannelsInfo` Data asset witch default information about channels. Set on init by ChatController.
Contain category of channels (eq. group "System" for Damage, Heal, ItemObtain channels), and its name, default color, etc.

Style based on 2 maps:
* `WindowsData` Map of data about window such as window position, opacity, owned tabs, etc. Key is WindowID.
* `TabsData` Map of data about tab such as tab name, listen channels, per channel style (color, font size), etc. Key is TabID.

Main functions: 
* `Apply` Broadcast settings update event and save settings.
* `ResetTabStyle` Reset style for tab.
* `RegisterNewTab` Create new tab with given parameters, notify ui for draw this tab.
* `RegisterDeleteTab` Delete tab, notify ui for remove tab widget.
* `RegisterMoveWindow` Save new window position. Must call after drag window.
* `RegisterResizeWindow` Save new window size. Must call after resize window.
*  &nbsp; Bunch of getters and setters for all parameters.

<a name="NS_Chat_Widgets"></a>
#### 2.1.3. Chat Widgets
`WChatSetupMessage`\
Window for editing ChatStyle. 
Initialized witch WindowID and TabID, if `TabID == INDEX_NONE` will be created a new tab.

![chat_style_settings](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/ChatStyleSettings.png)

`ChatWindow`\
Common chatting window, contain tabs.

![chat_window](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/ChatWindow.png)

`ChatSendMessage`\
Widget for type and send message, has 3 logical blocks:
* Select channel button
* Enter text block
* Add image button

On text changed search tags in text for replace it to decorator (ex. :Smile1: replace by < img id="Smile1"/>). Replacer decorator get by function `GetReplacerForTag`.

![chat_input](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/ChatInput.png)


<a name="NS_Minimap"></a>
### 2.2. Minimap
Work in progress.

Plans:
* Minimap
* GlobalMap
* Layers
* Interactive Icons







<br />
<br />
<br />
<br />
<br />

<a name="NS_Main_Old"></a>
## --Project bellow kinda very old and soon be refactored--

## Project features:

### 1. Team attitude

Team separating based on IGenericTeamAgentInterface with comfortable editing attitudes
via game settings.

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/Attitude.png)


### 2. Vote for next map

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/Vote.png)


### 3. Inventory and equipment based on Lyra sample

Inventory and equipment based on Lyra sample, 
where Item has a definition who store fragments of specific information.

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/ItemDef.png)

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/EquipmentDef.png)


### 4. Placing items

Grub and place special items, like turret or landmine

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/Landmine_2.png)

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/Landmine_1.png)


### 5. Unit Tests

Tests for inventory and equipment

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/Tests.png)

### 6. Enhanced Input

Input Actions for simple actions (movement and other)

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/InputMapping.png)

Input Actions with GameplayTag for send input events in GAS

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/InputActionWithTag.png)

### 7. Base Video settings

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/SettingsWidget.png)

Setting UI Description, like as Display mode, TipTool text etc.
Separate into SettingUIInfo class, and setup in project setting from Editor

![TestsExample](https://raw.githubusercontent.com/koenigigor/NetworkShooter/main/Images/AdditionalSettingsUIInfo.png)

##### 8. And other things...