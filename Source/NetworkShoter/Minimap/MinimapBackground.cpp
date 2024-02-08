// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapBackground.h"

#include "MapObject.h"
#include "MapObjectComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#if WITH_EDITORONLY_DATA
	#include "Engine/TextureRenderTarget2D.h"
	#include "AssetToolsModule.h"
	#include "AssetRegistry/AssetRegistryModule.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogMapBackgroungCapture, All, All);

AMinimapBackground::AMinimapBackground()
{
	PrimaryActorTick.bCanEverTick = false;

	MyRoot = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = MyRoot;

#if WITH_EDITORONLY_DATA
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
	SceneCapture->SetupAttachment(MyRoot);
	SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	CaptureBounds = CreateDefaultSubobject<UBoxComponent>("Bounds");
	CaptureBounds->SetupAttachment(MyRoot);
	CaptureBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
#endif

	MapObjectComponent = CreateDefaultSubobject<UMapObjectComponent>("MapObject");
	check(MapObjectComponent->MapObject);
	MapObjectComponent->MapObject->bIcon = false;
}

void AMinimapBackground::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITORONLY_DATA
	CaptureBounds->SetBoxExtent(FVector(Size / 2.f, Size / 2.f, 2.f));
	SceneCapture->OrthoWidth = Size;
	SceneCapture->TextureTarget = RenderTarget;
#endif
}


void AMinimapBackground::Capture()
{
#if WITH_EDITORONLY_DATA
	if (!RenderTarget) return;

	SceneCapture->CaptureScene();

	const auto MapObject = Cast<UMapObjectSimpleImage>(MapObjectComponent->MapObject);
	if (!MapObject)
	{
		UE_LOG(LogMapBackgroungCapture, Warning, TEXT("Capture expects UMapObjectSimpleImage derrived MapObject class"))
		return;
	}

	//override current image if exist
	if (bOverrideTexture && MapObject->Image)
	{
		RenderTarget->ConstructTexture2D(MapObject->Image->GetPackage(), MapObject->Image.GetName(), RenderTarget->GetMaskedFlags(), CTF_Default, nullptr);
		MapObject->Size = Size;
		MapObject->Image->MarkPackageDirty();
		return;
	}

	//create editor texture asset from render target, found in FAssetTypeActions_TextureRenderTarget::ExecuteCreateStatic
	FString Name;
	FString PackageName;
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(RenderTarget->GetOutermost()->GetName(), TEXT("_Tex"), PackageName, Name);

	// create a static 2d texture
	UTexture2D* NewTexture = RenderTarget->ConstructTexture2D(CreatePackage(*PackageName), Name, RenderTarget->GetMaskedFlags(), CTF_Default, nullptr);

	if (NewTexture)
	{
		// package needs saving
		NewTexture->MarkPackageDirty();

		// Notify the asset registry
		FAssetRegistryModule::AssetCreated(NewTexture);

		//Image = NewTexture;
		MapObject->Image = NewTexture;
		MapObject->Size = Size;
	}

	MarkPackageDirty();
#endif
}
