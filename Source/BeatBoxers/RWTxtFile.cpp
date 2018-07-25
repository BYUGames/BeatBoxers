// copyright 2017 BYU Animation

#include "RWTxtFile.h"




bool URWTxtFile::LoadTxt(FString FileNameA, FString& SaveTextA)
{
	return FFileHelper::LoadFileToString(SaveTextA, *(FileNameA));
}

bool URWTxtFile::SaveTxt(FString SaveTextB, FString FileNameB)
{
	return FFileHelper::SaveStringToFile(SaveTextB, *(FileNameB));
}