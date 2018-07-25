// copyright 2017 BYU Animation

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RWTxtFile.generated.h"

/**
 * 
 */
UCLASS()
class BEATBOXERS_API URWTxtFile : public UBlueprintFunctionLibrary
{
	GENERATED_BODY() public:

	UFUNCTION(BlueprintPure, Category = "Custom", meta = (Keywords = "LoadTxt"))
		static bool LoadTxt(FString FileNameA, FString& SaveTextA);

	UFUNCTION(BlueprintCallable, Category = "Custom", meta = (Keywords = "SaveTxt"))
		static bool SaveTxt(FString SaveTextB, FString FileNameB);
	
	
};
