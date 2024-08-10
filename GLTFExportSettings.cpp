#include "GLTFExportSettings.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool SaveConfigToFile(const FGLTFExportSettings& Settings, const FString& FilePath)
{
	FString JsonString;
	if (FJsonObjectConverter::UStructToJsonObjectString(Settings, JsonString))
	{
		return FFileHelper::SaveStringToFile(JsonString, *FilePath);
	}
	return false;
}

bool LoadConfigFromFile(FGLTFExportSettings& Settings, const FString& FilePath)
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &Settings, 0, 0);
	}
	return false;
}
