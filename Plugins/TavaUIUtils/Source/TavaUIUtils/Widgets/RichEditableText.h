#pragma once
#include "Components/EditableText.h"
#include "RichEditableText.generated.h"

class URichEditableTextDecorator;
class ITextDecorator;


/** Editable text block, witch support combined decorators (URichEditableTextDecorator) */
UCLASS()
class TAVAUIUTILS_API URichEditableText : public UEditableText
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Decorator")
	TArray<TSubclassOf<URichEditableTextDecorator>> DecoratorClasses;
	
	
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetFontSize(int32 InSize);

	const FTextBlockStyle& GetTextStyle() const;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
	void CreateDecorators(TArray<TSharedRef<ITextDecorator>>& OutDecorators);

	UPROPERTY(Transient)
	TArray<TObjectPtr<URichEditableTextDecorator>> InstanceDecorators;
};
