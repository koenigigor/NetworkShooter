// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapBackground.generated.h"

class UBoxComponent;
class UMinimapLayerCollider;
class UMapObjectComponent;

/** Minimap background image actor, represent map layer.
 *	In Editor, use Capture() for create render target image and set it to MapObject */
UCLASS()
class NETWORKSHOTER_API AMinimapBackground : public AActor
{
	GENERATED_BODY()
public:	
	AMinimapBackground();
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Capture render target image */
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Capture")
	void Capture();
	
protected:
#if WITH_EDITORONLY_DATA
	/** Override map texture if exist, else create new */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	bool bOverrideTexture = true;
	
	/** Size of background image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	float Size = 2400.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture")
	UTextureRenderTarget2D* RenderTarget = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* SceneCapture = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* CaptureBounds = nullptr;
#endif


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* MyRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMapObjectComponent* MapObjectComponent = nullptr;
};

