// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BeatBoxers.h"
#include "BBFunctionLibrary.generated.h"

class AActor;

/**
 * 
 */
UCLASS()
class BEATBOXERS_API UBBFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "BB Function Library")
	static FVector GetWorldPosition(AActor* Reference, FVector RelativePosition);
};
