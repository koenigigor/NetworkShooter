// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragDrop_DragChatTab.generated.h"

class UChatWindow;
class UChatTabButton;
class UChatScrollBox;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UDragDrop_DragChatTab : public UDragDropOperation
{
	GENERATED_BODY()
public:
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

	UFUNCTION(BlueprintCallable)
	void StartDragOverWindow(UChatWindow* ChatWindow);

	UFUNCTION(BlueprintCallable)
	void EndDragOverWindow();

	/** Owning chat window */
	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	UChatWindow* ChatWindow = nullptr;

	/** Owning chat window */
	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	UChatTabButton* Tab = nullptr;

	/** Owning chat window */
	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	UChatScrollBox* MessagesBox = nullptr;

	/** Canvas for attach window (can't get from window, because window remove from parent if remove last tab) */
	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
    UCanvasPanel* Canvas = nullptr;

protected:
	void DropOnCanvas(const FPointerEvent& PointerEvent);

	void ProcessDragOverWindow(const FPointerEvent& PointerEvent);

	void DropOnWindow(UChatWindow* Window);

	/** Window reverence if we drag over any chat window */
	TObjectPtr<UChatWindow> HoveredWindow = nullptr;

	/** Fake tab, visualize tab movement if drag hover window */
	UPROPERTY()
	UChatTabButton* FakeTab = nullptr;
};
