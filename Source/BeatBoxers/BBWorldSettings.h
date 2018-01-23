// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BBWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API ABBWorldSettings : public AWorldSettings
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Maximum camera displacement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=World)
	FBox CameraBounds;

	/** Padding to frame the subjects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=World)
	FVector2D CameraPadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=World)
	FVector InitialCameraLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=World)
	FVector InitialCameraLookAtLocation;
};
