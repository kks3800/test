#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GLTFExportSettings.generated.h"

USTRUCT(BlueprintType)
struct FGLTFExportSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Export Settings")
	FString ExportPath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Export Settings")
	FString ConfigFolderPath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Export Settings")
	TArray<FString> StaticMeshPaths;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Export Settings")
	TArray<FString> MaterialPaths;
};


bool SaveConfigToFile(const FGLTFExportSettings& Settings, const FString& FilePath);
bool LoadConfigFromFile(FGLTFExportSettings& Settings, const FString& FilePath);
