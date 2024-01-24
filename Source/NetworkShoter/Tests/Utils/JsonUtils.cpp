#include "JsonUtils.h"

#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY_STATIC(LogJsonUtils, All, All)

bool JsonUtils::WriteInputData(const FString& FileName, const FInputData& InputData)
{
	TSharedPtr<FJsonObject> MainJsonObject = FJsonObjectConverter::UStructToJsonObject(InputData);
	if (!MainJsonObject.IsValid())
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Fail convert struct to object"))
		return false;
	}

	FString OutputString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);

	if (!FJsonSerializer::Serialize(MainJsonObject.ToSharedRef(), JsonWriter))
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Fail serialalize json object to string"))
		return false;
	}

	if (!JsonWriter->Close())
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Json writer closing error"))
		return false;
	}

	if (!FFileHelper::SaveStringToFile(OutputString, *FileName))
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Fail save string in file %s"), *FileName);
		return false;
	}
	
	return true;
}

bool JsonUtils::ReadInputData(const FString& FileName, FInputData& InputData)
{
	TSharedPtr<FJsonObject> MainJsonObject = MakeShareable(new FJsonObject());
	
	FString InputString;
	if (!FFileHelper::LoadFileToString(InputString, *FileName))
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Fail load string fom file file %s"), *FileName);
		return false;
	}

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InputString);
	if (!FJsonSerializer::Deserialize(JsonReader, MainJsonObject))
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Fail deserialise json string from file %s"), *FileName);
		return false;		
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(MainJsonObject.ToSharedRef(), &InputData))
	{
		UE_LOG(LogJsonUtils, Error, TEXT("Fail convert json object from file %s, in struct"), *FileName);
		return false;	
	}
	
	return true;
}
