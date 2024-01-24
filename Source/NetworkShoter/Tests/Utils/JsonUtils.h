#pragma once
#include "InputData.h"


class JsonUtils
{
public:
	static bool WriteInputData(const FString& FileName, const FInputData& InputData);
	static bool ReadInputData(const FString& FileName, FInputData& InputData);
	
};
