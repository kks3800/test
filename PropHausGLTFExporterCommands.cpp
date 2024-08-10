#include "PropHausGLTFExporterCommands.h"

#define LOCTEXT_NAMESPACE "FPropHausGLTFExporterModule"

void FPropHausGLTFExporterCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "PropHausGLTFExporter", "Bring up PropHausGLTFExporter window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
