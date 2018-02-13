// copyright 2017 BYU Animation

#include "BBGameInstance.h"
#include "ConstructorHelpers.h"
#include "FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"

UBBGameInstance::UBBGameInstance(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UDataTable> MoveDataObj(TEXT("/Game/Data/MoveDataTable"));
	if (MoveDataObj.Succeeded() && MoveDataObj.Object != nullptr)
	{
		MoveData = MoveDataObj.Object;
	}
}

void UBBGameInstance::Init()
{
#if UE_BUILD_SHIPPING
	LoadData(TEXT("Data/MoveData.csv"), MoveData);
#endif
}

void UBBGameInstance::LoadData(FString CSVFile, UDataTable *DataTable)
{
	// A crucial part of this does not seem to work for shipping builds.
	// Let's just not use this for now.
	/*if (DataTable != nullptr)
	{
		FString CSVString;
		FString CSVPath = FPaths::GameContentDir() + CSVFile;

		if (FPaths::FileExists(CSVPath))
		{
			if (!FFileHelper::LoadFileToString(CSVString, *CSVPath))
			{
				UE_LOG(LogBeatBoxers, Error, TEXT("Failed to load %s"), *CSVPath);
			}
			else
			{

				TArray<FString> Errors = DataTable->CreateTableFromCSVString(CSVString);
				if (Errors.Num() > 0)
				{
					for (int i = 0; i < Errors.Num(); i++)
					{
						UE_LOG(LogBeatBoxers, Error, TEXT("Error loading data from %s: %s"), *CSVPath, *Errors[i]);
					}
				}
				else
				{
					UE_LOG(LogBeatBoxers, Log, TEXT("Loaded data from %s"), *CSVPath);
				}
			}
		}
		else
		{
			UE_LOG(LogBeatBoxers, Error, TEXT("Unable to find %s"), *CSVPath);
		}
	}
	else
	{
		UE_LOG(LogBeatBoxers, Error, TEXT("null table reference given for %s"), *CSVFile);
	}*/
}