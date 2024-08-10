#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FPropHausGLTFExporterStyle
{
public:

	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedRef<FSlateStyleSet> Create();

	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
