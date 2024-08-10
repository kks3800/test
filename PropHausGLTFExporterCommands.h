#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PropHausGLTFExporterStyle.h"

class FPropHausGLTFExporterCommands : public TCommands<FPropHausGLTFExporterCommands>
{
public:

    FPropHausGLTFExporterCommands()
        : TCommands<FPropHausGLTFExporterCommands>(TEXT("PropHausGLTFExporter"), NSLOCTEXT("Contexts", "PropHausGLTFExporter", "PropHausGLTFExporter Plugin"), NAME_None, FPropHausGLTFExporterStyle::GetStyleSetName())
    {
    }

    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
